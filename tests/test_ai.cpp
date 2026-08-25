#include <QtTest>

#include "aiopponent.h"
#include "gameengine.h"
#include "physicsengine.h"
#include "player.h"
#include "testutils.h"

#include <cmath>

class TestAI : public QObject
{
    Q_OBJECT

private slots:
    void exactShotLandsNearTarget();
    void windIsAccountedFor();
    void handlesBothFacings();
    void outOfRangeTargetReturnsValidShot();
    void errorScatterAroundTarget();
    void engineIntegrationTakesAiTurn();

private:
    static qreal landingFor(const QPointF &origin, qreal angle, qreal power,
                            int facing, qreal wind, qreal groundY);
};

qreal TestAI::landingFor(const QPointF &origin, qreal angle, qreal power,
                         int facing, qreal wind, qreal groundY)
{
    const PhysicsEngine physics;
    const auto r = physics.simulate(origin, angle, power, facing, wind, groundY);
    return r.hitGround ? r.landingX : qQNaN();
}

void TestAI::exactShotLandsNearTarget()
{
    const AIOpponent ai;
    const PhysicsEngine physics;
    const QPointF origin(112.0, 400.0);
    const qreal groundY = 420.0;

    for (const qreal targetX : { 300.0, 420.0, 550.0, 660.0 }) {
        const QPointF shot = ai.calculateShot(QPointF(origin), QPointF(targetX, 400.0),
                                               1, 0.0, groundY);
        QVERIFY(shot.x() >= 0.0 && shot.x() <= 90.0);
        QVERIFY(shot.y() >= 0.0 && shot.y() <= 100.0);
        const qreal landing = landingFor(origin, shot.x(), shot.y(), 1, 0.0, groundY);
        QVERIFY(qAbs(landing - targetX) < 12.0);
    }
}

void TestAI::windIsAccountedFor()
{
    const AIOpponent ai;
    const QPointF origin(112.0, 400.0);
    const qreal groundY = 420.0;
    const QPointF target(560.0, 400.0);

    const QPointF calm = ai.calculateShot(origin, target, 1, 0.0, groundY);
    const QPointF tailwind = ai.calculateShot(origin, target, 1, 60.0, groundY);
    const QPointF headwind = ai.calculateShot(origin, target, 1, -60.0, groundY);

    QVERIFY(qAbs(landingFor(origin, tailwind.x(), tailwind.y(), 1, 60.0, groundY)
                 - target.x()) < 12.0);
    QVERIFY(qAbs(landingFor(origin, headwind.x(), headwind.y(), 1, -60.0, groundY)
                 - target.x()) < 12.0);

    QVERIFY(tailwind.y() < calm.y() || tailwind.x() != calm.x());
    QVERIFY(headwind.y() > calm.y() || headwind.x() != calm.x());
}

void TestAI::handlesBothFacings()
{
    const AIOpponent ai;
    const qreal groundY = 420.0;

    const QPointF rightShot = ai.calculateShot(QPointF(112.0, 400.0),
                                               QPointF(600.0, 400.0), 1, 0.0,
                                               groundY);
    const QPointF leftShot = ai.calculateShot(QPointF(688.0, 400.0),
                                              QPointF(200.0, 400.0), -1, 0.0,
                                              groundY);

    QVERIFY(qAbs(landingFor(QPointF(112.0, 400.0), rightShot.x(), rightShot.y(),
                            1, 0.0, groundY) - 600.0) < 12.0);
    QVERIFY(qAbs(landingFor(QPointF(688.0, 400.0), leftShot.x(), leftShot.y(),
                            -1, 0.0, groundY) - 200.0) < 12.0);
}

void TestAI::outOfRangeTargetReturnsValidShot()
{
    const AIOpponent ai;
    const QPointF shot = ai.calculateShot(QPointF(112.0, 400.0),
                                          QPointF(2000.0, 400.0), 1, -60.0,
                                          420.0);

    QVERIFY(shot.x() >= 0.0 && shot.x() <= 90.0);
    QVERIFY(shot.y() >= 0.0 && shot.y() <= 100.0);

    const QPointF nearShot = ai.calculateShot(QPointF(112.0, 400.0),
                                              QPointF(-500.0, 400.0), 1, 60.0,
                                              420.0);
    QVERIFY(nearShot.x() >= 0.0 && nearShot.x() <= 90.0);
    QVERIFY(nearShot.y() >= 0.0 && nearShot.y() <= 100.0);
}

void TestAI::errorScatterAroundTarget()
{
    const AIOpponent ai;
    const QPointF origin(112.0, 400.0);
    const QPointF target(560.0, 400.0);
    const qreal groundY = 420.0;

    QVector<qreal> errors;
    qreal errorSum = 0.0;
    const int shots = 100;
    for (int i = 0; i < shots; ++i) {
        const QPointF shot = ai.calculateShotWithError(origin, target, 1, 0.0,
                                                       groundY);
        QVERIFY(shot.x() >= 0.0 && shot.x() <= 90.0);
        QVERIFY(shot.y() >= 0.0 && shot.y() <= 100.0);
        const qreal landing = landingFor(origin, shot.x(), shot.y(), 1, 0.0,
                                         groundY);
        errors.append(landing - target.x());
        errorSum += errors.last();
    }

    const qreal mean = errorSum / shots;
    QVERIFY2(qAbs(mean) < 20.0,
             qPrintable(QStringLiteral("mean landing error %1").arg(mean)));

    qreal minE = errors[0], maxE = errors[0];
    for (qreal e : std::as_const(errors)) {
        minE = qMin(minE, e);
        maxE = qMax(maxE, e);
    }
    QVERIFY2(maxE - minE > 10.0,
             qPrintable(QStringLiteral("shots do not scatter (spread %1..%2)")
                            .arg(minE).arg(maxE)));
}

void TestAI::engineIntegrationTakesAiTurn()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::VsAI);

    engine.aiTakeShot();
    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Aim);

    engine.fireProjectile();
    engine.resolveShot(false);
    QCOMPARE(engine.phase(), GameEngine::Phase::AITurn);

    engine.aiTakeShot();
    QCOMPARE(engine.phase(), GameEngine::Phase::Player2Fire);
    QVERIFY(engine.projectileInFlight());
    QVERIFY(engine.player2()->isAI());

    TestUtils::settleFlight(engine);
    QVERIFY(!engine.projectileInFlight());

    QVERIFY(engine.phase() == GameEngine::Phase::Player1Aim
            || engine.phase() == GameEngine::Phase::GameOver);
    if (engine.phase() == GameEngine::Phase::GameOver)
        QCOMPARE(engine.winner(), engine.player2());
}

QTEST_MAIN(TestAI)
#include "test_ai.moc"
