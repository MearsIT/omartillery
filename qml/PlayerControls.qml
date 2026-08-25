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
            spacing: 4

            Label {
                text: "ANGLE " + Math.round(angleSlider.value) + "\u00B0"
                color: Theme.green
                font.family: Theme.fontFamily
                font.pixelSize: 11
            }

            Slider {
                id: angleSlider
                objectName: "angleSlider"
                from: 0
                to: 90
                stepSize: 1
                enabled: controls.enabled
                onValueChanged: GameEngine.currentPlayer.angle = value

                background: Rectangle {
                    x: angleSlider.leftPadding
                    y: angleSlider.topPadding
                       + angleSlider.availableHeight / 2 - height / 2
                    width: angleSlider.availableWidth
                    height: 8
                    color: Theme.background
                    border.width: 2
                    border.color: Theme.dark

                    Rectangle {
                        width: angleSlider.visualPosition * parent.width
                        height: parent.height
                        color: Theme.green
                    }
                }

                handle: Rectangle {
                    x: angleSlider.leftPadding
                       + angleSlider.visualPosition
                         * (angleSlider.availableWidth - width)
                    y: angleSlider.topPadding
                       + angleSlider.availableHeight / 2 - height / 2
                    width: 16
                    height: 16
                    color: Theme.steel
                    border.width: 2
                    border.color: Theme.dark
                }

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

            Slider {
                id: powerSlider
                objectName: "powerSlider"
                from: 0
                to: 100
                stepSize: 1
                enabled: controls.enabled
                onValueChanged: GameEngine.currentPlayer.power = value

                background: Rectangle {
                    x: powerSlider.leftPadding
                    y: powerSlider.topPadding
                       + powerSlider.availableHeight / 2 - height / 2
                    width: powerSlider.availableWidth
                    height: 8
                    color: Theme.background
                    border.width: 2
                    border.color: Theme.dark

                    Rectangle {
                        width: powerSlider.visualPosition * parent.width
                        height: parent.height
                        color: Theme.green
                    }
                }

                handle: Rectangle {
                    x: powerSlider.leftPadding
                       + powerSlider.visualPosition
                         * (powerSlider.availableWidth - width)
                    y: powerSlider.topPadding
                       + powerSlider.availableHeight / 2 - height / 2
                    width: 16
                    height: 16
                    color: Theme.steel
                    border.width: 2
                    border.color: Theme.dark
                }

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

            background: Rectangle {
                implicitWidth: 120
                implicitHeight: 44
                color: parent.pressed ? Theme.chrome : Theme.background
                border.color: parent.enabled
                               ? (parent.hovered ? Theme.accent : Theme.green)
                               : Theme.dark
                border.width: 3
            }

            contentItem: Text {
                text: parent.text
                color: parent.enabled ? Theme.accent : Theme.dark
                font.family: Theme.fontFamily
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: GameEngine.fireProjectile()
        }
    }
}
