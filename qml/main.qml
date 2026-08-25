import QtQuick
import QtQuick.Controls
import QtMultimedia
import ArtilleryDuel

ApplicationWindow {
    id: root

    title: qsTr("Artillery Duel")
    width: 800
    height: 600
    minimumWidth: 640
    minimumHeight: 480
    visible: true
    color: Theme.background

    SoundEffect {
        id: fireSound
        source: "qrc:/ArtilleryDuel/assets/sounds/fire.wav"
        volume: 0.4
    }

    SoundEffect {
        id: explosionSound
        source: "qrc:/ArtilleryDuel/assets/sounds/explosion.wav"
        volume: 0.6
    }

    Item {
        id: viewport
        anchors.centerIn: parent
        width: 800
        height: 600
        scale: Math.min(root.width / 800, root.height / 600)

        StackView {
            id: stack
            anchors.fill: parent
            initialItem: mainMenu
        }

        Timer {
            id: aiThinkTimer
            interval: 900
            onTriggered: GameEngine.aiTakeShot()
        }

        Connections {
            target: GameEngine

            function onProjectileFired() {
                fireSound.play();
            }

            function onExplosionAt(x, y) {
                explosionSound.play();
            }

            function onPhaseChanged() {
                const phase = GameEngine.phase;
                const current = stack.currentItem ? stack.currentItem.objectName
                                                  : "";
                if (phase === GameEngine.Menu) {
                    if (current !== "mainMenu")
                        stack.replace(mainMenu);
                } else if (phase === GameEngine.GameOver) {
                    if (current !== "gameOverScreen")
                        stack.replace(gameOver);
                } else if (current !== "gamePage") {
                    stack.replace(gamePage);
                }
                if (phase === GameEngine.AITurn)
                    aiThinkTimer.restart();
            }
        }
    }

    Component {
        id: mainMenu
        MainMenu {}
    }

    Component {
        id: gameOver
        GameOverScreen {}
    }

    Component {
        id: gamePage

        Item {
            objectName: "gamePage"

            GameBoard {
                anchors.fill: parent
            }

            HUD {
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
            }

            PlayerControls {
                anchors {
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                anchors.bottomMargin: 10
            }
        }
    }
}
