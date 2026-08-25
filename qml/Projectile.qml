import QtQuick
import ArtilleryDuel

Image {
    id: projectile

    objectName: "projectile"
    source: "qrc:/ArtilleryDuel/assets/projectile.png"
    width: 8
    height: 8
    smooth: false
    visible: GameEngine.projectileInFlight
    x: GameEngine.projectileX - width / 2
    y: GameEngine.projectileY - height / 2
}
