import QtQuick
import QtQuick.Controls
import ArtilleryDuel

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
            text: (GameEngine.winner ? GameEngine.winner.name : "") + " WINS"
            color: Theme.accent
            font.family: Theme.fontFamily
            font.pixelSize: 18
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            objectName: "scoreLabel"
            text: GameEngine.player1.score + "  -  " + GameEngine.player2.score
            color: Theme.green
            font.family: Theme.fontFamily
            font.pixelSize: 14
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 10 }

        Button {
            id: overPlayAgainBtn
            objectName: "playAgainButton"
            text: qsTr("PLAY AGAIN")

            anchors.horizontalCenter: overPlayAgainBtn.horizontalCenter

            background: Rectangle {
                implicitWidth: 230
                implicitHeight: 44
                color: overPlayAgainBtn.pressed ? Theme.chrome : Theme.background
                border.color: overPlayAgainBtn.hovered ? Theme.accent : Theme.green
                border.width: 3
            }

            contentItem: Text {
                text: overPlayAgainBtn.text
                color: Theme.accent
                font.family: Theme.fontFamily
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: GameEngine.startGame(GameEngine.gameMode)
        }

        Button {
            id: overMenuBtn
            objectName: "menuButton"
            text: qsTr("MAIN MENU")

            anchors.horizontalCenter: overMenuBtn.horizontalCenter

            background: Rectangle {
                implicitWidth: 230
                implicitHeight: 44
                color: overMenuBtn.pressed ? Theme.chrome : Theme.background
                border.color: overMenuBtn.hovered ? Theme.accent : Theme.green
                border.width: 3
            }

            contentItem: Text {
                text: overMenuBtn.text
                color: Theme.accent
                font.family: Theme.fontFamily
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: GameEngine.returnToMenu()
        }
    }
}
