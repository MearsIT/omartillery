import QtQuick
import QtQuick.Controls
import ArtilleryDuel

FocusScope {
    id: controls

    objectName: "playerControls"

    readonly property bool interactive:
        GameEngine.phase === GameEngine.Player1Aim
        || GameEngine.phase === GameEngine.Player2Aim

    component AimSlider: Slider {
        id: aimSlider

        background: Rectangle {
            x: aimSlider.leftPadding
            y: aimSlider.topPadding + aimSlider.availableHeight / 2 - height / 2
            width: aimSlider.availableWidth
            height: 8
            color: Theme.background
            border.width: 2
            border.color: Theme.dark

            Rectangle {
                width: aimSlider.visualPosition * parent.width
                height: parent.height
                color: Theme.green
            }
        }

        handle: Rectangle {
            x: aimSlider.leftPadding
               + aimSlider.visualPosition
                 * (aimSlider.availableWidth - width)
            y: aimSlider.topPadding + aimSlider.availableHeight / 2 - height / 2
            width: 16
            height: 16
            color: Theme.steel
            border.width: 2
            border.color: Theme.dark
        }
    }

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
            GameEngine.currentPlayer.angle += 1;
            event.accepted = true;
            break;
        case Qt.Key_Right:
            GameEngine.currentPlayer.angle -= 1;
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
            spacing: 4

            Label {
                text: "ANGLE " + Math.round(angleSlider.value) + "\u00B0"
                color: Theme.green
                font.family: Theme.fontFamily
                font.pixelSize: 11
            }

            AimSlider {
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
            spacing: 4

            Label {
                text: "POWER " + Math.round(powerSlider.value)
                color: Theme.green
                font.family: Theme.fontFamily
                font.pixelSize: 11
            }

            AimSlider {
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

        RetroButton {
            id: fireButton
            objectName: "fireButton"
            panelWidth: 120
            text: qsTr("FIRE")
            enabled: controls.enabled
            onClicked: GameEngine.fireProjectile()
        }
    }
}
