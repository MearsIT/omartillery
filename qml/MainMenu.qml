import QtQuick
import QtQuick.Controls
import ArtilleryDuel

Item {
    id: menu

    objectName: "mainMenu"

    Rectangle {
        anchors.fill: parent
        color: Theme.background
    }

    Column {
        anchors.centerIn: parent
        spacing: 30

        Text {
            text: "ARTILLERY"
            color: Theme.accent
            font.family: Theme.fontFamily
            font.pixelSize: 36
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: "DUEL"
            color: Theme.accent
            font.family: Theme.fontFamily
            font.pixelSize: 36
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: qsTr("A 1983 CLASSIC, REBORN")
            color: "#7869C4"
            font.family: Theme.fontFamily
            font.pixelSize: 10
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 16 }

        Button {
            objectName: "twoPlayerButton"
            text: qsTr("2 PLAYER")

            anchors.horizontalCenter: parent.horizontalCenter

            background: Rectangle {
                implicitWidth: 230
                implicitHeight: 44
                color: parent.pressed ? Theme.chrome : Theme.background
                border.color: parent.hovered ? Theme.accent : Theme.green
                border.width: 3
            }

            contentItem: Text {
                text: parent.text
                color: Theme.accent
                font.family: Theme.fontFamily
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: GameEngine.startGame(GameEngine.TwoPlayer)
        }

        Button {
            objectName: "vsAiButton"
            text: qsTr("VS COMPUTER")

            anchors.horizontalCenter: parent.horizontalCenter

            background: Rectangle {
                implicitWidth: 230
                implicitHeight: 44
                color: parent.pressed ? Theme.chrome : Theme.background
                border.color: parent.hovered ? Theme.accent : Theme.green
                border.width: 3
            }

            contentItem: Text {
                text: parent.text
                color: Theme.accent
                font.family: Theme.fontFamily
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: GameEngine.startGame(GameEngine.VsAI)
        }

        Text {
            text: qsTr("ARROWS: AIM   SPACE: FIRE")
            color: Theme.green
            font.family: Theme.fontFamily
            font.pixelSize: 9
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
