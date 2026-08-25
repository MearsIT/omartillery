import QtQuick
import QtQuick.Controls
import ArtilleryDuel

Item {
    id: menu

    objectName: "mainMenu"

    Rectangle {
        anchors.fill: parent
        color: "#40318D"
    }

    Column {
        anchors.centerIn: parent
        spacing: 28

        Text {
            text: "ARTILLERY DUEL"
            color: "#BFCE72"
            font.family: "monospace"
            font.bold: true
            font.pixelSize: 42
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: qsTr("A 1983 CLASSIC, REBORN")
            color: "#7869C4"
            font.family: "monospace"
            font.pixelSize: 13
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 10 }

        Button {
            objectName: "twoPlayerButton"
            text: qsTr("2 PLAYER")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: GameEngine.startGame(GameEngine.TwoPlayer)
        }

        Button {
            objectName: "vsAiButton"
            text: qsTr("VS COMPUTER")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: GameEngine.startGame(GameEngine.VsAI)
        }

        Text {
            text: qsTr("ARROWS: AIM   SPACE: FIRE")
            color: "#94E089"
            font.family: "monospace"
            font.pixelSize: 12
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
