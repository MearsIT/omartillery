pragma ModuleName: "io.github.jandal.artillery-duel"

import QtQuick
import QtQuick.Controls
import QtMultimedia
import io.github.jandal.artillery-duel

Item {
    id: root

    // Panel plugins receive their surface from the shell
    // No window properties needed - fill parent surface
    anchors.fill: parent

    // Panel lifecycle API methods required by Omarchy shell
    function open() {
        root.visible = true;
    }

    function close() {
        root.visible = false;
    }

    function toggle() {
        root.visible = !root.visible;
    }

    function closeForPopoutSwitch() {
        // Close for popout transition - same as close for this plugin
        root.visible = false;
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.background

        SoundEffect {
            id: fireSound
            source: "assets/sounds/fire.wav"
            volume: 0.4
        }

        SoundEffect {
            id: explosionSound
            source: "assets/sounds/explosion.wav"
            volume: 0.6
        }

        Item {
            id: viewport
            anchors.centerIn: parent
            width: GameEngine.boardWidth
            height: GameEngine.boardHeight
            scale: Math.min(parent.width / GameEngine.boardWidth,
                            parent.height / GameEngine.boardHeight)

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
}
