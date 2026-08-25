import QtQuick
import QtQuick.Controls
import ArtilleryDuel

FocusScope {
    id: controls

    objectName: "playerControls"

    readonly property bool interactive:
        GameEngine.phase === GameEngine.Player1Aim
        || GameEngine.phase === GameEngine.Player2Aim

    focus: interactive
    enabled: interactive
    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    function syncSliders()
    {
        if (!angleSlider.pressed)
            angleSlider.value = GameEngine.currentPlayer.angle;
        if (!powerSlider.pressed)
            powerSlider.value = GameEngine.currentPlayer.power;
    }

    Component.onCompleted: syncSliders()

    Connections {
        target: GameEngine
        function onCurrentPlayerChanged() { controls.syncSliders(); }
    }

    Keys.onPressed: function(event) {
        switch (event.key) {
        case Qt.Key_Left:
            GameEngine.currentPlayer.angle -= 1;
            event.accepted = true;
            break;
        case Qt.Key_Right:
            GameEngine.currentPlayer.angle += 1;
            event.accepted = true;
            break;
        case Qt.Key_Down:
            GameEngine.currentPlayer.power -= 1;
            event.accepted = true;
            break;
        case Qt.Key_Up:
            GameEngine.currentPlayer.power += 1;
            event.accepted = true;
            break;
        case Qt.Key_Space:
            if (controls.enabled)
                GameEngine.fireProjectile();
            event.accepted = true;
            break;
        }
    }

    Row {
        id: row
        spacing: 20

        Column {
            spacing: 2

            Label {
                text: "ANGLE " + Math.round(angleSlider.value) + "\u00B0"
                color: "#94E089"
                font.family: "monospace"
                font.bold: true
            }

            Slider {
                id: angleSlider
                objectName: "angleSlider"
                from: 0
                to: 90
                stepSize: 1
                enabled: controls.enabled
                onValueChanged: GameEngine.currentPlayer.angle = value

                Connections {
                    target: GameEngine.currentPlayer
                    function onAngleChanged() {
                        if (!angleSlider.pressed)
                            angleSlider.value = GameEngine.currentPlayer.angle;
                    }
                }
            }
        }

        Column {
            spacing: 2

            Label {
                text: "POWER " + Math.round(powerSlider.value)
                color: "#94E089"
                font.family: "monospace"
                font.bold: true
            }

            Slider {
                id: powerSlider
                objectName: "powerSlider"
                from: 0
                to: 100
                stepSize: 1
                enabled: controls.enabled
                onValueChanged: GameEngine.currentPlayer.power = value

                Connections {
                    target: GameEngine.currentPlayer
                    function onPowerChanged() {
                        if (!powerSlider.pressed)
                            powerSlider.value = GameEngine.currentPlayer.power;
                    }
                }
            }
        }

        Button {
            id: fireButton
            objectName: "fireButton"
            text: qsTr("FIRE")
            enabled: controls.enabled
            onClicked: GameEngine.fireProjectile()
        }
    }
}
