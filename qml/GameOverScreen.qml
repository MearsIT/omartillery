
import QtQuick
import omartillery

Item {
    id: over

    objectName: "gameOverScreen"

    Rectangle {
        anchors.fill: parent
        color: Theme.background
    }

    Column {
        anchors.centerIn: parent
        spacing: 28

        Text {
            text: qsTr("GAME OVER")
            color: Theme.red
            font.family: Theme.fontFamily
            font.pixelSize: 32
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            objectName: "winnerLabel"
            text: (GameModel.winner ? GameModel.winner.name : "") + " WINS"
            color: Theme.accent
            font.family: Theme.fontFamily
            font.pixelSize: 18
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            objectName: "scoreLabel"
            text: GameModel.player1.score + "  -  " + GameModel.player2.score
            color: Theme.green
            font.family: Theme.fontFamily
            font.pixelSize: 14
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 10 }

        RetroButton {
            objectName: "playAgainButton"
            text: qsTr("PLAY AGAIN")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: GameModel.startGame(GameModel.gameMode)
        }

        RetroButton {
            objectName: "menuButton"
            text: qsTr("MAIN MENU")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: GameModel.returnToMenu()
        }
    }
}
