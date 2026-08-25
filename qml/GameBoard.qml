
import QtQuick
import omartillery

Item {
    id: board

    width: GameModel.boardWidth
    height: GameModel.boardHeight
    clip: true

    Rectangle {
        id: sky
        anchors.fill: parent
        color: Theme.background
    }

    Canvas {
        id: terrainCanvas
        anchors.fill: parent
        antialiasing: false

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const heights = GameModel.terrainHeights;
            if (heights.length === 0)
                return;

            const columnWidth = width / heights.length;
            ctx.beginPath();
            for (let i = 0; i < heights.length; ++i) {
                const groundY = heights[i];
                ctx.fillStyle = "#94E089";
                ctx.fillRect(i * columnWidth, groundY, columnWidth + 0.5, 3);
                ctx.rect(i * columnWidth, groundY + 3, columnWidth + 0.5,
                         height - groundY - 3);
            }
            ctx.fillStyle = "#8B5429";
            ctx.fill();
            ctx.fillStyle = ctx.createPattern(
                        "../assets/terrain_pattern.png",
                        "repeat");
            ctx.fill();
            ctx.fillStyle = "#574200";
            ctx.fillRect(0, height - 24, width, 24);
        }

        Connections {
            target: GameModel
            function onTerrainChanged() { terrainCanvas.requestPaint(); }
        }
    }

    Tank {
        player: GameModel.player1
    }

    Tank {
        player: GameModel.player2
    }

    Projectile {}

    Explosion {
        id: explosionFx

        Connections {
            target: GameModel
            function onExplosionAt(x, y) {
                explosionFx.x = x - explosionFx.width / 2;
                explosionFx.y = y - explosionFx.height / 2;
                explosionFx.restart();
            }
        }

        onDone: GameModel.explosionFinished()
    }

    FrameAnimation {
        running: GameModel.projectileInFlight
        onTriggered: GameModel.updateFlight(GameModel.projectileTime + frameTime)
    }
}
