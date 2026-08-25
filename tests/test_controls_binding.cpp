#include <QtTest>

#include "gameengine.h"
#include "player.h"
#include "testutils.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>

class TestControlsBinding : public QObject
{
    Q_OBJECT

private slots:
    void slidersUpdateEngine();
    void fireButtonClickFires();
    void keyboardAdjustsAndFires();
    void arrowKeyClampingAtBounds();
    void disabledDuringNonAimPhases();
    void sliderValuesClamped();
    void sliderResyncOnTurnSwitch();

private:
    QQuickItem *createControls(QQmlEngine &qmlEngine, QScopedPointer<QObject> &root);
};

using TestUtils::engineSingleton;

QQuickItem *TestControlsBinding::createControls(QQmlEngine &qmlEngine,
                                                QScopedPointer<QObject> &root)
{
    QQmlComponent component(&qmlEngine, QUrl(QStringLiteral(
            "qrc:/ArtilleryDuel/qml/PlayerControls.qml")));
    if (component.status() != QQmlComponent::Ready)
        return nullptr;
    root.reset(component.create());
    return qobject_cast<QQuickItem *>(root.get());
}

void TestControlsBinding::slidersUpdateEngine()
{
    QQmlEngine qmlEngine;
    QScopedPointer<QObject> root;
    QQuickItem *controls = createControls(qmlEngine, root);
    QVERIFY(controls != nullptr);

    GameEngine *engine = engineSingleton(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::TwoPlayer);
    QCOMPARE(engine->phase(), GameEngine::Phase::Player1Aim);
    QTRY_VERIFY(controls->isEnabled());

    auto *angleSlider = controls->findChild<QQuickItem *>("angleSlider");
    auto *powerSlider = controls->findChild<QQuickItem *>("powerSlider");
    QVERIFY(angleSlider != nullptr);
    QVERIFY(powerSlider != nullptr);

    angleSlider->setProperty("value", 30.0);
    QTRY_COMPARE(engine->currentPlayer()->angle(), 30.0);

    powerSlider->setProperty("value", 77.0);
    QTRY_COMPARE(engine->currentPlayer()->power(), 77.0);
}

void TestControlsBinding::fireButtonClickFires()
{
    QQmlEngine qmlEngine;
    QScopedPointer<QObject> root;
    QQuickItem *controls = createControls(qmlEngine, root);
    QVERIFY(controls != nullptr);

    GameEngine *engine = engineSingleton(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::TwoPlayer);

    auto *fireButton = controls->findChild<QQuickItem *>("fireButton");
    QVERIFY(fireButton != nullptr);
    QVERIFY(QMetaObject::invokeMethod(fireButton, "clicked"));
    QTRY_COMPARE(engine->phase(), GameEngine::Phase::Player1Fire);
}

void TestControlsBinding::keyboardAdjustsAndFires()
{
    QQmlEngine qmlEngine;
    QScopedPointer<QObject> root;
    QQuickItem *controls = createControls(qmlEngine, root);
    QVERIFY(controls != nullptr);

    GameEngine *engine = engineSingleton(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::TwoPlayer);

    QQuickWindow window;
    window.resize(400, 120);
    controls->setParentItem(window.contentItem());
    controls->setFocus(true);
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));
    QTRY_VERIFY(controls->hasActiveFocus());

    const qreal baseAngle = engine->currentPlayer()->angle();
    const qreal basePower = engine->currentPlayer()->power();

    QTest::keyClick(&window, Qt::Key_Left);
    QTRY_COMPARE(engine->currentPlayer()->angle(), baseAngle + 1.0);

    QTest::keyClick(&window, Qt::Key_Up);
    QTRY_COMPARE(engine->currentPlayer()->power(), basePower + 1.0);

    QTest::keyClick(&window, Qt::Key_Right);
    QTRY_COMPARE(engine->currentPlayer()->angle(), baseAngle);

    QTest::keyClick(&window, Qt::Key_Down);
    QTRY_COMPARE(engine->currentPlayer()->power(), basePower);

    QTest::keyClick(&window, Qt::Key_Space);
    QTRY_COMPARE(engine->phase(), GameEngine::Phase::Player1Fire);
}

void TestControlsBinding::arrowKeyClampingAtBounds()
{
    QQmlEngine qmlEngine;
    QScopedPointer<QObject> root;
    QQuickItem *controls = createControls(qmlEngine, root);
    QVERIFY(controls != nullptr);

    GameEngine *engine = engineSingleton(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::TwoPlayer);

    QQuickWindow window;
    window.resize(400, 120);
    controls->setParentItem(window.contentItem());
    controls->setFocus(true);
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));
    QTRY_VERIFY(controls->hasActiveFocus());

    // Happy path: Left arrow increases angle from 45 to 46
    engine->currentPlayer()->setAngle(45.0);
    QTRY_COMPARE(engine->currentPlayer()->angle(), 45.0);
    QTest::keyClick(&window, Qt::Key_Left);
    QTRY_COMPARE(engine->currentPlayer()->angle(), 46.0);

    // Happy path: Right arrow decreases angle from 45 to 44
    engine->currentPlayer()->setAngle(45.0);
    QTRY_COMPARE(engine->currentPlayer()->angle(), 45.0);
    QTest::keyClick(&window, Qt::Key_Right);
    QTRY_COMPARE(engine->currentPlayer()->angle(), 44.0);

    // Edge case: Left arrow at 90 degrees does not increase (clamped)
    engine->currentPlayer()->setAngle(90.0);
    QTRY_COMPARE(engine->currentPlayer()->angle(), 90.0);
    QTest::keyClick(&window, Qt::Key_Left);
    QTRY_COMPARE(engine->currentPlayer()->angle(), 90.0);

    // Edge case: Right arrow at 0 degrees does not decrease (clamped)
    engine->currentPlayer()->setAngle(0.0);
    QTRY_COMPARE(engine->currentPlayer()->angle(), 0.0);
    QTest::keyClick(&window, Qt::Key_Right);
    QTRY_COMPARE(engine->currentPlayer()->angle(), 0.0);
}

void TestControlsBinding::disabledDuringNonAimPhases()
{
    QQmlEngine qmlEngine;
    QScopedPointer<QObject> root;
    QQuickItem *controls = createControls(qmlEngine, root);
    QVERIFY(controls != nullptr);

    GameEngine *engine = engineSingleton(qmlEngine);
    QVERIFY(engine != nullptr);

    QTRY_VERIFY(!controls->isEnabled());

    engine->startGame(GameEngine::GameMode::TwoPlayer);
    QTRY_VERIFY(controls->isEnabled());

    engine->fireProjectile();
    QTRY_VERIFY(!controls->isEnabled());
    QCOMPARE(engine->phase(), GameEngine::Phase::Player1Fire);

    engine->resolveShot(false);
    QTRY_VERIFY(controls->isEnabled());

    engine->returnToMenu();
    QTRY_VERIFY(!controls->isEnabled());
}

void TestControlsBinding::sliderValuesClamped()
{
    QQmlEngine qmlEngine;
    QScopedPointer<QObject> root;
    QQuickItem *controls = createControls(qmlEngine, root);
    QVERIFY(controls != nullptr);

    GameEngine *engine = engineSingleton(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::TwoPlayer);

    auto *angleSlider = controls->findChild<QQuickItem *>("angleSlider");
    auto *powerSlider = controls->findChild<QQuickItem *>("powerSlider");
    QVERIFY(angleSlider != nullptr);
    QVERIFY(powerSlider != nullptr);

    angleSlider->setProperty("value", 500.0);
    powerSlider->setProperty("value", -50.0);

    QTRY_COMPARE(engine->currentPlayer()->angle(), 90.0);
    QTRY_COMPARE(engine->currentPlayer()->power(), 0.0);
}

void TestControlsBinding::sliderResyncOnTurnSwitch()
{
    QQmlEngine qmlEngine;
    QScopedPointer<QObject> root;
    QQuickItem *controls = createControls(qmlEngine, root);
    QVERIFY(controls != nullptr);

    GameEngine *engine = engineSingleton(qmlEngine);
    QVERIFY(engine != nullptr);
    engine->startGame(GameEngine::GameMode::TwoPlayer);

    auto *angleSlider = controls->findChild<QQuickItem *>("angleSlider");
    auto *powerSlider = controls->findChild<QQuickItem *>("powerSlider");
    QVERIFY(angleSlider != nullptr);
    QVERIFY(powerSlider != nullptr);

    engine->currentPlayer()->setAngle(30.0);
    engine->currentPlayer()->setPower(80.0);
    QTRY_COMPARE(angleSlider->property("value").toReal(), 30.0);
    QTRY_COMPARE(powerSlider->property("value").toReal(), 80.0);

    engine->fireProjectile();
    engine->resolveShot(false);
    QTRY_COMPARE(engine->phase(), GameEngine::Phase::Player2Aim);
    QTRY_COMPARE(angleSlider->property("value").toReal(), 45.0);
    QTRY_COMPARE(powerSlider->property("value").toReal(), 50.0);
}

QTEST_MAIN(TestControlsBinding)
#include "test_controls_binding.moc"
