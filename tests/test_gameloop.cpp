#include <QtTest>

#include "aiopponent.h"
#include "gameengine.h"
#include "physicsengine.h"
#include "testutils.h"
#include "player.h"

#include <QQmlApplicationEngine>
#include <QDateTime>
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
    void qmlDrivenFlightResolvesTurn();

private:
    static GameEngine *loadUi(QQmlApplicationEngine &qmlEngine);
};

using TestUtils::findClearShot;
using TestUtils::settleFlight;

GameEngine *TestGameLoop::loadUi(QQmlApplicationEngine &qmlEngine)
{
    qmlEngine.load(QUrl(QStringLiteral("qrc:/ArtilleryDuel/qml/main.qml")));
    if (qmlEngine.rootObjects().isEmpty())
        return nullptr;
    return TestUtils::engineSingleton(qmlEngine);
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
    int consecutiveBlocked = 0;
    while (engine->phase() != GameEngine::Phase::GameOver && turns < 60) {
        QVERIFY(engine->phase() == GameEngine::Phase::Player1Aim
                || engine->phase() == GameEngine::Phase::Player2Aim);

        qreal angle = 0.0;
        qreal power = 0.0;
        if (engine->currentPlayer() == engine->player1()) {
            if (!findClearShot(*engine, angle, power)) {
                if (++consecutiveBlocked >= 10) {
                    QVERIFY2(++regenerations <= 12, "no completable terrain found");
                    engine->startGame(GameEngine::GameMode::TwoPlayer);
                    turns = 0;
                    consecutiveBlocked = 0;
                    continue;
                }
                angle = 75.0;
                power = 30.0;
            } else {
                consecutiveBlocked = 0;
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
    QVERIFY(turns <= 20);

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
        QVERIFY2(++attempts <= 30, "no clear shot on any terrain");
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

void TestGameLoop::qmlDrivenFlightResolvesTurn()
{
    QQmlApplicationEngine qmlEngine;
    GameEngine *engine = loadUi(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::TwoPlayer);
    engine->setWind(0.0);

    auto *window = qobject_cast<QQuickWindow *>(qmlEngine.rootObjects().first());
    QVERIFY(window != nullptr);
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));

    engine->currentPlayer()->setAngle(45.0);
    engine->currentPlayer()->setPower(60.0);
    engine->fireProjectile();
    QVERIFY(engine->projectileInFlight());

    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + 15000;
    while (engine->phase() == GameEngine::Phase::Player1Fire
           && QDateTime::currentMSecsSinceEpoch() < deadline) {
        QTest::qWait(50);
    }

    QVERIFY2(engine->phase() != GameEngine::Phase::Player1Fire,
             "QML-driven flight never resolved: FrameAnimation/Explosion wiring broken");
    QVERIFY(engine->phase() == GameEngine::Phase::Player2Aim
            || engine->phase() == GameEngine::Phase::GameOver);
    QVERIFY(!engine->projectileInFlight());
}

QTEST_MAIN(TestGameLoop)
#include "test_gameloop.moc"
