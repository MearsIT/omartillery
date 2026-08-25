#include <QtTest>

#include "aiopponent.h"
#include "gameengine.h"
#include "physicsengine.h"
#include "player.h"

#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>

class TestGameLoop : public QObject
{
    Q_OBJECT

private slots:
    void menuLaunchesGameAndBack();
    void completeTwoPlayerGame();
    void completeVsAIGame();
    void hudReflectsGameState();

private:
    static bool findClearShot(const GameEngine &engine, qreal &angle,
                              qreal &power);
    static void settleFlight(GameEngine &engine);
    GameEngine *loadUi(QQmlApplicationEngine &qmlEngine);
};

GameEngine *TestGameLoop::loadUi(QQmlApplicationEngine &qmlEngine)
{
    qmlEngine.load(QUrl(QStringLiteral("qrc:/ArtilleryDuel/qml/main.qml")));
    if (qmlEngine.rootObjects().isEmpty())
        return nullptr;
    const int typeId = qmlTypeId("ArtilleryDuel", 1, 0, "GameEngine");
    if (typeId < 0)
        return nullptr;
    return qmlEngine.singletonInstance<GameEngine *>(typeId);
}

bool TestGameLoop::findClearShot(const GameEngine &engine, qreal &angle,
                                 qreal &power)
{
    const PhysicsEngine physics;
    const Player *shooter = engine.currentPlayer();
    const Player *target = shooter == engine.player1() ? engine.player2()
                                                       : engine.player1();
    const int facing = shooter->facing();
    const QPointF center(shooter->x(), shooter->y() - 8.0);
    const QPointF boxCenter(target->x(), target->y() - 8.0);
    const qreal wind = engine.wind();

    for (int a = 75; a >= 20; a -= 5) {
        const QPointF origin(center.x() + facing * 12.0 * qCos(qDegreesToRadians(a)),
                             center.y() - 12.0 * qSin(qDegreesToRadians(a)));
        for (int p = 15; p <= 100; ++p) {
            bool terrainFirst = false;
            bool boxHit = false;
            const auto sim = physics.simulate(origin, a, p, facing, wind,
                                              GameEngine::BOARD_HEIGHT + 60.0);
            const qreal tEnd = qMin(sim.hitGround ? sim.flightTime : 4.0, 4.0);
            for (qreal t = 0.0; t <= tEnd && !terrainFirst && !boxHit;
                 t += 1.0 / 480.0) {
                const QPointF q = physics.positionAt(origin, a, p, facing, wind, t);
                if (physics.pointIntersectsBox(q, boxCenter, 12.0, 12.0))
                    boxHit = true;
                else if (q.y() >= engine.terrainHeightAt(q.x()) - 2.0)
                    terrainFirst = true;
            }
            if (boxHit && !terrainFirst) {
                angle = a;
                power = p;
                return true;
            }
        }
    }
    return false;
}

void TestGameLoop::settleFlight(GameEngine &engine)
{
    qreal t = 0.0;
    while (engine.projectileInFlight() && t < 12.0) {
        t += 1.0 / 60.0;
        engine.updateFlight(t);
    }
    if (engine.phase() == GameEngine::Phase::Player1Fire
        || engine.phase() == GameEngine::Phase::Player2Fire)
        engine.explosionFinished();
}

void TestGameLoop::menuLaunchesGameAndBack()
{
    QQmlApplicationEngine qmlEngine;
    GameEngine *engine = loadUi(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->returnToMenu();

    auto *window = qobject_cast<QQuickWindow *>(qmlEngine.rootObjects().first());
    QVERIFY(window != nullptr);
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    auto *menu = window->findChild<QQuickItem *>("mainMenu");
    QVERIFY(menu != nullptr);
    QVERIFY(menu->isVisible());

    auto *twoPlayer = window->findChild<QQuickItem *>("twoPlayerButton");
    QVERIFY(twoPlayer != nullptr);
    QVERIFY(QMetaObject::invokeMethod(twoPlayer, "clicked"));
    QTRY_COMPARE(engine->phase(), GameEngine::Phase::Player1Aim);

    auto *page = window->findChild<QQuickItem *>("gamePage");
    QVERIFY(page != nullptr);
    QVERIFY(page->isVisible());
    QVERIFY(window->findChild<QQuickItem *>("hud") != nullptr);
    QVERIFY(window->findChild<QQuickItem *>("playerControls") != nullptr);

    engine->returnToMenu();
    QTRY_VERIFY(window->findChild<QQuickItem *>("mainMenu")->isVisible());
    QVERIFY(!page->isVisible());
}

void TestGameLoop::completeTwoPlayerGame()
{
    QQmlApplicationEngine qmlEngine;
    GameEngine *engine = loadUi(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::TwoPlayer);

    auto *window = qobject_cast<QQuickWindow *>(qmlEngine.rootObjects().first());
    QVERIFY(window != nullptr);
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    int turns = 0;
    int regenerations = 0;
    while (engine->phase() != GameEngine::Phase::GameOver && turns < 30) {
        QVERIFY(engine->phase() == GameEngine::Phase::Player1Aim
                || engine->phase() == GameEngine::Phase::Player2Aim);

        qreal angle = 0.0;
        qreal power = 0.0;
        if (engine->currentPlayer() == engine->player1()) {
            if (!findClearShot(*engine, angle, power)) {
                QVERIFY2(++regenerations <= 12, "no completable terrain found");
                engine->startGame(GameEngine::GameMode::TwoPlayer);
                turns = 0;
                continue;
            }
        } else {
            angle = 75.0;
            power = 30.0;
        }
        engine->currentPlayer()->setAngle(angle);
        engine->currentPlayer()->setPower(power);
        engine->fireProjectile();
        QVERIFY(engine->phase() == GameEngine::Phase::Player1Fire
                || engine->phase() == GameEngine::Phase::Player2Fire);
        settleFlight(*engine);
        ++turns;
    }

    QCOMPARE(engine->phase(), GameEngine::Phase::GameOver);
    QVERIFY(engine->winner() != nullptr);
    QCOMPARE(engine->winner()->score(), 1);
    QVERIFY(turns <= 4);

    auto *over = window->findChild<QQuickItem *>("gameOverScreen");
    QTRY_VERIFY(over != nullptr && over->isVisible());

    auto *winnerLabel = window->findChild<QQuickItem *>("winnerLabel");
    QVERIFY(winnerLabel != nullptr);
    QVERIFY(winnerLabel->property("text").toString()
            .contains(engine->winner()->name()));

    auto *playAgain = window->findChild<QQuickItem *>("playAgainButton");
    QVERIFY(QMetaObject::invokeMethod(playAgain, "clicked"));
    QTRY_COMPARE(engine->phase(), GameEngine::Phase::Player1Aim);
    QCOMPARE(engine->gameMode(), GameEngine::GameMode::TwoPlayer);

    auto *menuButton = window->findChild<QQuickItem *>("menuButton");
    QVERIFY(menuButton != nullptr);
    engine->returnToMenu();
    QTRY_VERIFY(window->findChild<QQuickItem *>("mainMenu") != nullptr);
}

void TestGameLoop::completeVsAIGame()
{
    QQmlApplicationEngine qmlEngine;
    GameEngine *engine = loadUi(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::VsAI);

    auto *window = qobject_cast<QQuickWindow *>(qmlEngine.rootObjects().first());
    QVERIFY(window != nullptr);
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    int aiTurns = 0;
    int turns = 0;
    int regenerations = 0;
    int consecutiveBlocked = 0;
    while (engine->phase() != GameEngine::Phase::GameOver && turns < 60) {
        if (engine->phase() == GameEngine::Phase::AITurn) {
            ++aiTurns;
            engine->aiTakeShot();
            QCOMPARE(engine->phase(), GameEngine::Phase::Player2Fire);
            settleFlight(*engine);
        } else if (engine->phase() == GameEngine::Phase::Player1Aim) {
            qreal angle = 0.0;
            qreal power = 0.0;
            if (!findClearShot(*engine, angle, power)) {
                if (++consecutiveBlocked >= 10) {
                    QVERIFY2(++regenerations <= 12, "no completable terrain found");
                    engine->startGame(GameEngine::GameMode::VsAI);
                    aiTurns = 0;
                    turns = 0;
                    consecutiveBlocked = 0;
                    continue;
                }
                angle = 75.0;
                power = 30.0;
            } else {
                consecutiveBlocked = 0;
            }
            engine->currentPlayer()->setAngle(angle);
            engine->currentPlayer()->setPower(power);
            engine->fireProjectile();
            settleFlight(*engine);
        }
        ++turns;
    }

    QCOMPARE(engine->phase(), GameEngine::Phase::GameOver);
    QVERIFY(engine->winner() != nullptr);
    QVERIFY(aiTurns >= 1);

    auto *over = window->findChild<QQuickItem *>("gameOverScreen");
    QTRY_VERIFY(over != nullptr && over->isVisible());
}

void TestGameLoop::hudReflectsGameState()
{
    QQmlApplicationEngine qmlEngine;
    GameEngine *engine = loadUi(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::VsAI);

    auto *window = qobject_cast<QQuickWindow *>(qmlEngine.rootObjects().first());
    QVERIFY(window != nullptr);
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    auto *turnLabel = window->findChild<QQuickItem *>("hudTurnLabel");
    auto *windLabel = window->findChild<QQuickItem *>("hudWindLabel");
    auto *bar1 = window->findChild<QQuickItem *>("healthBar1");
    auto *bar2 = window->findChild<QQuickItem *>("healthBar2");
    auto *fill1 = window->findChild<QQuickItem *>("healthFill1");
    auto *fill2 = window->findChild<QQuickItem *>("healthFill2");
    QVERIFY(turnLabel != nullptr);
    QVERIFY(windLabel != nullptr);
    QVERIFY(bar1 != nullptr);
    QVERIFY(bar2 != nullptr);
    QVERIFY(fill1 != nullptr);
    QVERIFY(fill2 != nullptr);

    QTRY_VERIFY(turnLabel->property("text").toString()
                .contains("Player 1: AIM & FIRE"));
    QVERIFY(windLabel->property("text").toString().startsWith("WIND"));
    QTRY_COMPARE(fill1->width(), bar1->width());
    QTRY_COMPARE(fill2->width(), bar2->width());

    qreal angle = 0.0;
    qreal power = 0.0;
    int attempts = 0;
    while (!findClearShot(*engine, angle, power)) {
        QVERIFY2(++attempts <= 8, "no clear shot on any terrain");
        engine->startGame(GameEngine::GameMode::VsAI);
    }
    engine->currentPlayer()->setAngle(angle);
    engine->currentPlayer()->setPower(power);
    engine->fireProjectile();
    settleFlight(*engine);

    QCOMPARE(engine->player2()->health(),
             Player::MAX_HEALTH - GameEngine::HIT_DAMAGE);
    QTRY_COMPARE(fill2->width(), bar2->width() / 2);
}

QTEST_MAIN(TestGameLoop)
#include "test_gameloop.moc"
