pragma ModuleName: "io.github.jandal.artillery-duel"

import QtQuick
import io.github.jandal.artillery-duel

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
        switch (GameEngine.phase) {
        case GameEngine.Player1Aim:
            return GameEngine.player1.name + ": AIM & FIRE";
        case GameEngine.Player1Fire:
            return GameEngine.player1.name + ": FIRING";
        case GameEngine.Player2Aim:
            return GameEngine.player2.name + ": AIM & FIRE";
        case GameEngine.Player2Fire:
            return GameEngine.player2.name + ": FIRING";
        case GameEngine.AITurn:
            return GameEngine.player2.name + " CALCULATING";
        default:
            return "";
        }
    }

    function windArrows() {
        const wind = GameEngine.wind;
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
        player: GameEngine.player1
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
            text: GameEngine.wind === 0
                  ? qsTr("WIND CALM")
                  : qsTr("WIND ") + hud.windArrows() + " "
                    + Math.abs(Math.round(GameEngine.wind))
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
        player: GameEngine.player2
        mirrored: true
    }
}
