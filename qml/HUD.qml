pragma ModuleName: "omartillery"

import QtQuick
import omartillery

Rectangle {
    id: hud

    objectName: "hud"
    height: 52
    color: Theme.chrome

    component HealthColumn: Column {
        id: healthColumn

        required property var player
        property bool mirrored: false
        spacing: 3

        Text {
            text: healthColumn.mirrored
                  ? healthColumn.player.score + "  " + healthColumn.player.name
                  : healthColumn.player.name + "  " + healthColumn.player.score
            color: Theme.accent
            font.family: Theme.fontFamily
            font.pixelSize: 11
        }

        Item {
            objectName: healthColumn.mirrored ? "healthBar2" : "healthBar1"
            width: 120
            height: 10

            Rectangle {
                anchors.fill: parent
                color: Theme.background
            }

            Rectangle {
                objectName: healthColumn.mirrored ? "healthFill2" : "healthFill1"
                anchors {
                    left: healthColumn.mirrored ? undefined : parent.left
                    right: healthColumn.mirrored ? parent.right : undefined
                    top: parent.top
                    bottom: parent.bottom
                }
                width: parent.width * healthColumn.player.health / 100
                color: healthColumn.player.health > 50 ? Theme.green
                                                       : Theme.red
            }
        }
    }

    function phaseLabel() {
        switch (GameModel.phase) {
        case GameModel.Player1Aim:
            return GameModel.player1.name + ": AIM & FIRE";
        case GameModel.Player1Fire:
            return GameModel.player1.name + ": FIRING";
        case GameModel.Player2Aim:
            return GameModel.player2.name + ": AIM & FIRE";
        case GameModel.Player2Fire:
            return GameModel.player2.name + ": FIRING";
        case GameModel.AITurn:
            return GameModel.player2.name + " CALCULATING";
        default:
            return "";
        }
    }

    function windArrows() {
        const wind = GameModel.wind;
        const count = Math.max(1, Math.round(Math.abs(wind) / 15));
        const glyph = wind > 0 ? ">" : "<";
        return glyph.repeat(count);
    }

    HealthColumn {
        anchors {
            left: parent.left
            leftMargin: 10
            verticalCenter: parent.verticalCenter
        }
        player: GameModel.player1
    }

    Column {
        spacing: 3
        anchors.centerIn: parent

        Text {
            objectName: "hudTurnLabel"
            text: hud.phaseLabel()
            color: "#FFFFFF"
            font.family: Theme.fontFamily
            font.pixelSize: 12
        }

        Text {
            objectName: "hudWindLabel"
            text: GameModel.wind === 0
                  ? qsTr("WIND CALM")
                  : qsTr("WIND ") + hud.windArrows() + " "
                    + Math.abs(Math.round(GameModel.wind))
            color: Theme.green
            font.family: Theme.fontFamily
            font.pixelSize: 10
        }
    }

    HealthColumn {
        anchors {
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
        player: GameModel.player2
        mirrored: true
    }
}
