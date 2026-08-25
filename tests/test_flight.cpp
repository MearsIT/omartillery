#include <QtTest>

#include "gameengine.h"
#include "physicsengine.h"
#include "player.h"
#include "testutils.h"

class TestFlight : public QObject
{
    Q_OBJECT

private slots:
    void fireLaunchesFlightFromMuzzle();
    void positionMatchesPhysicsAnalytically();
    void terrainImpactExplodesThenAdvancesAfterAnimation();
    void directHitDamagesOpponent();
    void offscreenMissResolvesImmediately();
    void shooterImmuneToOwnShot();
};

void TestFlight::fireLaunchesFlightFromMuzzle()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);
    engine.setWind(0.0);
    engine.currentPlayer()->setAngle(45.0);
    engine.currentPlayer()->setPower(50.0);

    QSignalSpy firedSpy(&engine, &GameEngine::projectileFired);
    engine.fireProjectile();

    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Fire);
    QVERIFY(engine.projectileInFlight());
    QCOMPARE(firedSpy.count(), 1);
    QCOMPARE(engine.projectileTime(), 0.0);

    const QPointF muzzle = TestUtils::muzzlePos(engine.player1(), 45.0);
    QVERIFY(qAbs(engine.projectileX() - muzzle.x()) < 0.01);
    QVERIFY(qAbs(engine.projectileY() - muzzle.y()) < 0.01);
}

void TestFlight::positionMatchesPhysicsAnalytically()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);
    engine.setWind(-20.0);
    engine.currentPlayer()->setPower(70.0);

    const PhysicsEngine physics;

    for (int angleDeg = 30; angleDeg <= 80; angleDeg += 10) {
        engine.setWind(-20.0);
        const Player *shooter = engine.currentPlayer();
        const int facing = shooter->facing();
        engine.currentPlayer()->setAngle(angleDeg);
        engine.currentPlayer()->setPower(70.0);
        engine.fireProjectile();

        const QPointF origin = TestUtils::muzzlePos(shooter, angleDeg);

        qreal t = 0.0;
        while (t < 0.4) {
            t += 1.0 / 60.0;
            engine.updateFlight(t);
            if (!engine.projectileInFlight())
                break;
            const QPointF expected = physics.positionAt(origin, angleDeg, 70.0,
                                                        facing, -20.0, t);
            QVERIFY(qAbs(engine.projectileX() - expected.x()) < 0.01);
            QVERIFY(qAbs(engine.projectileY() - expected.y()) < 0.01);
        }
        if (t >= 0.4)
            return;

        if (engine.phase() == GameEngine::Phase::Player1Fire
            || engine.phase() == GameEngine::Phase::Player2Fire)
            engine.explosionFinished();
    }
    QFAIL("no launch angle kept the projectile airborne for 0.4 s");
}

void TestFlight::terrainImpactExplodesThenAdvancesAfterAnimation()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);
    engine.setWind(0.0);
    engine.currentPlayer()->setAngle(80.0);
    engine.currentPlayer()->setPower(20.0);

    QSignalSpy explosionSpy(&engine, &GameEngine::explosionAt);
    QSignalSpy flightDoneSpy(&engine, &GameEngine::flightFinished);

    engine.fireProjectile();
    TestUtils::stepFlight(engine, 1.0 / 60.0, 600);

    QVERIFY(!engine.projectileInFlight());
    QCOMPARE(explosionSpy.count(), 1);
    QCOMPARE(flightDoneSpy.count(), 0);

    const qreal ex = explosionSpy.at(0).at(0).toReal();
    const qreal ey = explosionSpy.at(0).at(1).toReal();
    QVERIFY(ex > 0.0 && ex < GameEngine::BOARD_WIDTH);
    QVERIFY(qAbs(ey - engine.terrainHeightAt(ex)) < 12.0);

    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Fire);

    engine.explosionFinished();
    QCOMPARE(engine.phase(), GameEngine::Phase::Player2Aim);
    QCOMPARE(flightDoneSpy.count(), 1);
    QCOMPARE(engine.player2()->health(), Player::MAX_HEALTH);
}

void TestFlight::directHitDamagesOpponent()
{
    GameEngine engine;

    bool found = false;
    for (int attempt = 0; attempt < 12 && !found; ++attempt) {
        engine.startGame(GameEngine::GameMode::TwoPlayer);
        engine.setWind(0.0);

        qreal angle = 0.0;
        qreal power = 0.0;
        if (!TestUtils::findClearShot(engine, angle, power))
            continue;

        QSignalSpy explosionSpy(&engine, &GameEngine::explosionAt);
        engine.currentPlayer()->setAngle(angle);
        engine.currentPlayer()->setPower(power);
        engine.fireProjectile();
        TestUtils::stepFlight(engine, 1.0 / 120.0, 1200);

        QVERIFY(!engine.projectileInFlight());
        QCOMPARE(explosionSpy.count(), 1);
        engine.explosionFinished();
        QCOMPARE(engine.player2()->health(),
                 Player::MAX_HEALTH - GameEngine::HIT_DAMAGE);
        QCOMPARE(engine.phase(), GameEngine::Phase::Player2Aim);
        found = true;
    }
    QVERIFY2(found, "no clear trajectory found to opponent");
}

void TestFlight::offscreenMissResolvesImmediately()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);

    QSignalSpy explosionSpy(&engine, &GameEngine::explosionAt);
    QSignalSpy flightDoneSpy(&engine, &GameEngine::flightFinished);

    int explosions = 0;
    int misses = 0;
    for (int angle = 0; angle <= 90; angle += 15) {
        for (int power = 0; power <= 100; power += 25) {
            for (const qreal wind : { -60.0, 0.0, 60.0 }) {
                if (engine.phase() != GameEngine::Phase::Player1Aim
                    && engine.phase() != GameEngine::Phase::Player2Aim)
                    continue;

                engine.setWind(wind);
                engine.currentPlayer()->setAngle(angle);
                engine.currentPlayer()->setPower(power);
                engine.fireProjectile();
                TestUtils::stepFlight(engine, 1.0 / 60.0, 1200);

                QVERIFY2(!engine.projectileInFlight(),
                         "flight did not terminate within 20 s of simulated time");
                if (engine.phase() == GameEngine::Phase::Player1Fire
                    || engine.phase() == GameEngine::Phase::Player2Fire) {
                    engine.explosionFinished();
                    ++explosions;
                } else {
                    ++misses;
                }
                QVERIFY(engine.phase() == GameEngine::Phase::Player1Aim
                        || engine.phase() == GameEngine::Phase::Player2Aim
                        || engine.phase() == GameEngine::Phase::GameOver);
            }
        }
    }
    QVERIFY(explosions > 0);
    QVERIFY(explosions + misses == flightDoneSpy.count());
}

void TestFlight::shooterImmuneToOwnShot()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);
    engine.setWind(0.0);
    engine.currentPlayer()->setAngle(50.0);
    engine.currentPlayer()->setPower(10.0);

    engine.fireProjectile();
    TestUtils::stepFlight(engine, 1.0 / 60.0, 600);

    QVERIFY(!engine.projectileInFlight());
    QCOMPARE(engine.player1()->health(), Player::MAX_HEALTH);
}

QTEST_MAIN(TestFlight)
#include "test_flight.moc"
