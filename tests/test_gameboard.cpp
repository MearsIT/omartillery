#include <QtTest>

#include "gameengine.h"
#include "player.h"

#include <QImage>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>

class TestGameBoard : public QObject
{
    Q_OBJECT

private slots:
    void tanksFollowPlayerCoordinates();
    void rendersTerrainAndSky();

private:
    static GameEngine *engineSingleton(QQmlEngine &engine);
};

GameEngine *TestGameBoard::engineSingleton(QQmlEngine &engine)
{
    const int typeId = qmlTypeId("ArtilleryDuel", 1, 0, "GameEngine");
    if (typeId < 0)
        return nullptr;
    return engine.singletonInstance<GameEngine *>(typeId);
}

void TestGameBoard::tanksFollowPlayerCoordinates()
{
    QQmlEngine qmlEngine;
    QQmlComponent component(&qmlEngine,
                            QUrl(QStringLiteral(
                                     "qrc:/ArtilleryDuel/qml/GameBoard.qml")));

    QVERIFY2(component.status() == QQmlComponent::Ready,
             qPrintable(component.errorString()));
    QScopedPointer<QObject> root(component.create());
    QVERIFY(root != nullptr);

    GameEngine *engine = engineSingleton(qmlEngine);
    QVERIFY2(engine != nullptr, "GameEngine singleton unavailable");
    engine->startGame(GameEngine::GameMode::TwoPlayer);

    auto *tankRight = root->findChild<QQuickItem *>("tank_right");
    auto *tankLeft = root->findChild<QQuickItem *>("tank_left");
    QVERIFY(tankRight != nullptr);
    QVERIFY(tankLeft != nullptr);

    QCOMPARE(tankRight->width(), 16.0);
    QCOMPARE(tankRight->height(), 16.0);
    QTRY_COMPARE(tankRight->x(), engine->player1()->x() - 8.0);
    QTRY_COMPARE(tankRight->y(), engine->player1()->y() - 16.0);
    QTRY_COMPARE(tankLeft->x(), engine->player2()->x() - 8.0);
    QTRY_COMPARE(tankLeft->y(), engine->player2()->y() - 16.0);

    engine->player1()->setX(200.0);
    QTRY_COMPARE(tankRight->x(), 192.0);

    QVERIFY(tankLeft->x() > tankRight->x());
}

void TestGameBoard::rendersTerrainAndSky()
{
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/ArtilleryDuel/qml/main.qml")));
    QCOMPARE(engine.rootObjects().size(), 1);
    auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().first());
    QVERIFY(window != nullptr);

    const int typeId = qmlTypeId("ArtilleryDuel", 1, 0, "GameEngine");
    auto *gameEngine = typeId >= 0
            ? engine.singletonInstance<GameEngine *>(typeId) : nullptr;
    QVERIFY2(gameEngine != nullptr, "GameEngine singleton unavailable");
    gameEngine->startGame(GameEngine::GameMode::TwoPlayer);

    window->resize(800, 600);
    window->show();
    QVERIFY(QTest::qWaitForWindowExposed(window));
    QTest::qWait(250);

    const QImage frame = window->grabWindow();
    QCOMPARE(frame.size(), QSize(800, 600));

    const QColor sky = frame.pixelColor(400, 100);
    QVERIFY2(qAbs(sky.red() - 0x40) < 4 && qAbs(sky.green() - 0x31) < 4
                 && qAbs(sky.blue() - 0x8D) < 4,
             qPrintable(QStringLiteral("sky color was %1").arg(sky.name())));

    const QColor bedrock = frame.pixelColor(400, 592);
    QVERIFY2(qAbs(bedrock.red() - 0x57) < 4 && qAbs(bedrock.green() - 0x42) < 4
                 && qAbs(bedrock.blue() - 0x00) < 4,
             qPrintable(QStringLiteral("bedrock color was %1").arg(bedrock.name())));

    bool sawTerrain = false;
    for (int y = 340; y < 570 && !sawTerrain; ++y) {
        const QColor c = frame.pixelColor(80, y);
        if (qAbs(c.red() - 0x94) < 6 && qAbs(c.green() - 0xE0) < 6
            && qAbs(c.blue() - 0x89) < 6)
            sawTerrain = true;
    }
    QVERIFY2(sawTerrain, "no green terrain surface found in expected band");
}

QTEST_MAIN(TestGameBoard)
#include "test_gameboard.moc"
