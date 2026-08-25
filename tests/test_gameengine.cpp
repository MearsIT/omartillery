#include <QtTest>

#include "gameengine.h"
#include "player.h"

#include <QQmlEngine>
#include <QQmlComponent>

class TestGameEngine : public QObject
{
    Q_OBJECT

private slots:
    void initialState();
    void startGameTwoPlayer();
    void startGameVsAI();
    void fireRequiresAimPhase();
    void turnCycleTwoPlayer();
    void turnCycleVsAI();
    void winBySecondHit();
    void windStaysInRange();
    void terrainShapeAndSampling();
    void modeSwitchMidGame();
    void qmlPropertyAccess();

private:
    bool spawnAndResolveCycle(GameEngine &engine, int cycles);
};

void TestGameEngine::initialState()
{
    GameEngine engine;
    QCOMPARE(engine.phase(), GameEngine::Phase::Menu);
    QVERIFY(engine.player1() != nullptr);
    QVERIFY(engine.player2() != nullptr);
    QCOMPARE(engine.player1()->health(), Player::MAX_HEALTH);
    QCOMPARE(engine.player2()->health(), Player::MAX_HEALTH);
    QCOMPARE(engine.player1()->name(), QStringLiteral("Player 1"));
    QCOMPARE(engine.player2()->name(), QStringLiteral("Player 2"));
    QVERIFY(!engine.player1()->isAI());
    QVERIFY(!engine.player2()->isAI());
    QCOMPARE(engine.winner(), nullptr);
}

void TestGameEngine::startGameTwoPlayer()
{
    GameEngine engine;
    QSignalSpy phaseSpy(&engine, &GameEngine::phaseChanged);
    engine.startGame(GameEngine::GameMode::TwoPlayer);

    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Aim);
    QCOMPARE(engine.currentPlayer(), engine.player1());
    QVERIFY(phaseSpy.count() >= 1);

    const QVariantList terrain = engine.terrainHeights();
    QCOMPARE(terrain.size(), GameEngine::TERRAIN_COLUMNS);
    for (const QVariant &v : terrain) {
        QVERIFY(v.toReal() >= 340.0);
        QVERIFY(v.toReal() <= 560.0);
    }

    QVERIFY(engine.player1()->x() < engine.player2()->x());
    QCOMPARE(engine.player1()->facing(), 1);
    QCOMPARE(engine.player2()->facing(), -1);
    QCOMPARE(qreal(engine.player1()->y()), engine.terrainHeightAt(engine.player1()->x()));
    QVERIFY(!engine.player2()->isAI());
}

void TestGameEngine::startGameVsAI()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::VsAI);

    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Aim);
    QVERIFY(engine.player2()->isAI());
    QVERIFY(!engine.player1()->isAI());
    QCOMPARE(engine.player2()->name(), QStringLiteral("CPU"));
}

void TestGameEngine::fireRequiresAimPhase()
{
    GameEngine engine;
    engine.fireProjectile();
    QCOMPARE(engine.phase(), GameEngine::Phase::Menu);

    engine.startGame(GameEngine::GameMode::TwoPlayer);
    engine.fireProjectile();
    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Fire);

    engine.fireProjectile();
    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Fire);

    engine.resolveShot(false);
    QCOMPARE(engine.phase(), GameEngine::Phase::Player2Aim);
}

void TestGameEngine::turnCycleTwoPlayer()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);

    engine.fireProjectile();
    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Fire);
    engine.resolveShot(false);
    QCOMPARE(engine.phase(), GameEngine::Phase::Player2Aim);
    QCOMPARE(engine.currentPlayer(), engine.player2());

    engine.fireProjectile();
    QCOMPARE(engine.phase(), GameEngine::Phase::Player2Fire);
    engine.resolveShot(false);
    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Aim);
    QCOMPARE(engine.currentPlayer(), engine.player1());
}

void TestGameEngine::turnCycleVsAI()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::VsAI);

    engine.fireProjectile();
    engine.resolveShot(false);
    QCOMPARE(engine.phase(), GameEngine::Phase::AITurn);
    QCOMPARE(engine.currentPlayer(), engine.player2());

    engine.fireProjectile();
    QCOMPARE(engine.phase(), GameEngine::Phase::Player2Fire);
    engine.resolveShot(false);
    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Aim);
    QCOMPARE(engine.currentPlayer(), engine.player1());
}

void TestGameEngine::winBySecondHit()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);
    QSignalSpy gameOverSpy(&engine, &GameEngine::gameOver);

    engine.fireProjectile();
    engine.resolveShot(true);
    QCOMPARE(engine.player2()->health(), Player::MAX_HEALTH - GameEngine::HIT_DAMAGE);
    QCOMPARE(engine.phase(), GameEngine::Phase::Player2Aim);

    engine.fireProjectile();
    engine.resolveShot(false);

    engine.fireProjectile();
    engine.resolveShot(true);
    QCOMPARE(engine.player2()->health(), 0);
    QVERIFY(!engine.player2()->alive());
    QCOMPARE(engine.phase(), GameEngine::Phase::GameOver);
    QCOMPARE(engine.winner(), engine.player1());
    QCOMPARE(engine.player1()->score(), 1);
    QCOMPARE(gameOverSpy.count(), 1);

    engine.resolveShot(true);
    QCOMPARE(engine.phase(), GameEngine::Phase::GameOver);

    engine.startGame(GameEngine::GameMode::TwoPlayer);
    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Aim);
    QCOMPARE(engine.player2()->health(), Player::MAX_HEALTH);
    QCOMPARE(engine.winner(), nullptr);
}

bool TestGameEngine::spawnAndResolveCycle(GameEngine &engine, int cycles)
{
    for (int i = 0; i < cycles; ++i) {
        if (engine.phase() != GameEngine::Phase::Player1Aim
            && engine.phase() != GameEngine::Phase::Player2Aim)
            return false;
        engine.fireProjectile();
        engine.resolveShot(false);
    }
    return true;
}

void TestGameEngine::windStaysInRange()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);
    QVERIFY(spawnAndResolveCycle(engine, 40));
    QVERIFY(engine.wind() >= -GameEngine::WIND_MAX);
    QVERIFY(engine.wind() <= GameEngine::WIND_MAX);
}

void TestGameEngine::terrainShapeAndSampling()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);

    QCOMPARE(engine.terrainHeightAt(-50.0), engine.terrainHeightAt(0.0));
    QCOMPARE(engine.terrainHeightAt(9000.0), engine.terrainHeightAt(800.0));

    const qreal sampled = engine.terrainHeightAt(400.0);
    QVERIFY(sampled >= 340.0);
    QVERIFY(sampled <= 560.0);

    const qreal p1Pad = engine.terrainHeightAt(engine.player1()->x() + 8.0);
    const qreal p1Center = engine.terrainHeightAt(engine.player1()->x());
    QCOMPARE(p1Pad, p1Center);
}

void TestGameEngine::modeSwitchMidGame()
{
    GameEngine engine;
    engine.startGame(GameEngine::GameMode::TwoPlayer);
    engine.fireProjectile();
    engine.resolveShot(false);

    engine.startGame(GameEngine::GameMode::VsAI);
    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Aim);
    QVERIFY(engine.player2()->isAI());

    engine.fireProjectile();
    engine.resolveShot(false);
    QCOMPARE(engine.phase(), GameEngine::Phase::AITurn);

    engine.startGame(GameEngine::GameMode::TwoPlayer);
    QVERIFY(!engine.player2()->isAI());
    QCOMPARE(engine.player2()->name(), QStringLiteral("Player 2"));
    QCOMPARE(engine.phase(), GameEngine::Phase::Player1Aim);
}

void TestGameEngine::qmlPropertyAccess()
{
    QQmlEngine qmlEngine;
    QQmlComponent component(&qmlEngine);
    component.setData(R"(
        import QtQuick
        import ArtilleryDuel

        QtObject {
            property int phase: GameEngine.phase
            property int mode: GameEngine.TwoPlayer
            property real angle: GameEngine.currentPlayer.angle
            property real power: GameEngine.currentPlayer.power
            property int p1Health: GameEngine.player1.health
            property real wind: GameEngine.wind
            property var terrain: GameEngine.terrainHeights
            function beginGame() { GameEngine.startGame(GameEngine.VsAI) }
            function aim(a, p) { GameEngine.currentPlayer.angle = a; GameEngine.currentPlayer.power = p }
            function clampProbe(a, p) { GameEngine.currentPlayer.angle = a; GameEngine.currentPlayer.power = p }
        }
    )", QUrl());

    QVERIFY2(component.status() == QQmlComponent::Ready,
             qPrintable(component.errorString()));
    QScopedPointer<QObject> root(component.create());
    QVERIFY(root != nullptr);

    QCOMPARE(root->property("phase").toInt(), int(GameEngine::Phase::Menu));

    QMetaObject::invokeMethod(root.get(), "beginGame");
    QCOMPARE(root->property("phase").toInt(), int(GameEngine::Phase::Player1Aim));
    QCOMPARE(root->property("p1Health").toInt(), Player::MAX_HEALTH);
    QCOMPARE(root->property("terrain").toList().size(), GameEngine::TERRAIN_COLUMNS);
    QVERIFY(root->property("wind").toReal() >= -GameEngine::WIND_MAX);
    QVERIFY(root->property("wind").toReal() <= GameEngine::WIND_MAX);

    QMetaObject::invokeMethod(root.get(), "aim",
                              Q_ARG(QVariant, QVariant(30.0)),
                              Q_ARG(QVariant, QVariant(75.0)));
    QCOMPARE(root->property("angle").toReal(), 30.0);
    QCOMPARE(root->property("power").toReal(), 75.0);

    QMetaObject::invokeMethod(root.get(), "clampProbe",
                              Q_ARG(QVariant, QVariant(140.0)),
                              Q_ARG(QVariant, QVariant(-5.0)));
    QCOMPARE(root->property("angle").toReal(), 90.0);
    QCOMPARE(root->property("power").toReal(), 0.0);
}

QTEST_MAIN(TestGameEngine)
#include "test_gameengine.moc"
