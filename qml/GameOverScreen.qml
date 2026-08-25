import QtQuick
import QtQuick.Controls
import ArtilleryDuel

Item {
    id: over

    objectName: "gameOverScreen"

    Rectangle {
        anchors.fill: parent
        color: "#40318D"
    }

    Column {
        anchors.centerIn: parent
        spacing: 26

        Text {
            text: qsTr("GAME OVER")
            color: "#B86962"
            font.family: "monospace"
            font.bold: true
            font.pixelSize: 44
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            objectName: "winnerLabel"
            text: (GameEngine.winner ? GameEngine.winner.name : "") + " WINS"
            color: "#BFCE72"
            font.family: "monospace"
            font.bold: true
            font.pixelSize: 22
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            objectName: "scoreLabel"
            text: GameEngine.player1.score + "  -  " + GameEngine.player2.score
            color: "#94E089"
            font.family: "monospace"
            font.pixelSize: 18
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 8 }

        Button {
            objectName: "playAgainButton"
            text: qsTr("PLAY AGAIN")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: GameEngine.startGame(GameEngine.gameMode)
        }

        Button {
            objectName: "menuButton"
            text: qsTr("MAIN MENU")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: GameEngine.returnToMenu()
        }
    }
}
