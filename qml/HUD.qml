import QtQuick
import ArtilleryDuel

Rectangle {
    id: hud

    objectName: "hud"
    height: 52
    color: "#5C47E4"

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
        let arrows = "";
        for (let i = 0; i < count; ++i)
            arrows += glyph;
        return arrows;
    }

    Row {
        anchors {
            left: parent.left
            leftMargin: 10
            verticalCenter: parent.verticalCenter
        }
        spacing: 8

        Column {
            spacing: 3

            Text {
                text: GameEngine.player1.name + "  " + GameEngine.player1.score
                color: "#BFCE72"
                font.family: "monospace"
                font.bold: true
                font.pixelSize: 13
            }

            Item {
                objectName: "healthBar1"
                width: 120
                height: 10

                Rectangle {
                    anchors.fill: parent
                    color: "#40318D"
                }

                Rectangle {
                    objectName: "healthFill1"
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: parent.width * GameEngine.player1.health / 100
                    color: GameEngine.player1.health > 50 ? "#94E089" : "#B86962"
                }
            }
        }
    }

    Column {
        spacing: 3
        anchors.centerIn: parent

        Text {
            objectName: "hudTurnLabel"
            text: hud.phaseLabel()
            color: "#FFFFFF"
            font.family: "monospace"
            font.bold: true
            font.pixelSize: 14
        }

        Text {
            objectName: "hudWindLabel"
            text: GameEngine.wind === 0
                  ? qsTr("WIND CALM")
                  : qsTr("WIND ") + hud.windArrows() + " "
                    + Math.abs(Math.round(GameEngine.wind))
            color: "#94E089"
            font.family: "monospace"
            font.pixelSize: 12
        }
    }

    Row {
        anchors {
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
        spacing: 8
        layoutDirection: Qt.RightToLeft

        Column {
            spacing: 3

            Text {
                text: GameEngine.player2.score + "  " + GameEngine.player2.name
                color: "#BFCE72"
                font.family: "monospace"
                font.bold: true
                font.pixelSize: 13
            }

            Item {
                objectName: "healthBar2"
                width: 120
                height: 10

                Rectangle {
                    anchors.fill: parent
                    color: "#40318D"
                }

                Rectangle {
                    objectName: "healthFill2"
                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: parent.width * GameEngine.player2.health / 100
                    color: GameEngine.player2.health > 50 ? "#94E089" : "#B86962"
                }
            }
        }
    }
}
