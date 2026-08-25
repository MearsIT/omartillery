
import QtQuick
import QtQuick.Controls
import omartillery

FocusScope {
    id: controls

    objectName: "playerControls"

    readonly property bool interactive:
        GameModel.phase === GameModel.Player1Aim
        || GameModel.phase === GameModel.Player2Aim

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
            angleSlider.value = GameModel.currentPlayer.angle;
        if (!powerSlider.pressed)
            powerSlider.value = GameModel.currentPlayer.power;
    }

    Component.onCompleted: syncSliders()

    Connections {
        target: GameModel
        function onCurrentPlayerChanged() { controls.syncSliders(); }
    }

    Keys.onPressed: function(event) {
        switch (event.key) {
        case Qt.Key_Left:
            GameModel.currentPlayer.angle += 1;
            event.accepted = true;
            break;
        case Qt.Key_Right:
            GameModel.currentPlayer.angle -= 1;
            event.accepted = true;
            break;
        case Qt.Key_Down:
            GameModel.currentPlayer.power -= 1;
            event.accepted = true;
            break;
        case Qt.Key_Up:
            GameModel.currentPlayer.power += 1;
            event.accepted = true;
            break;
        case Qt.Key_Space:
            if (controls.enabled)
                GameModel.fireProjectile();
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
                onValueChanged: GameModel.currentPlayer.angle = value

                Connections {
                    target: GameModel.currentPlayer
                    function onAngleChanged() {
                        if (!angleSlider.pressed)
                            angleSlider.value = GameModel.currentPlayer.angle;
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
                onValueChanged: GameModel.currentPlayer.power = value

                Connections {
                    target: GameModel.currentPlayer
                    function onPowerChanged() {
                        if (!powerSlider.pressed)
                            powerSlider.value = GameModel.currentPlayer.power;
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
            onClicked: GameModel.fireProjectile()
        }
    }
}
