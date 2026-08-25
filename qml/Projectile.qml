
import QtQuick
import omartillery

Image {
    id: projectile

    objectName: "projectile"
    source: "../assets/projectile.png"
    width: 8
    height: 8
    smooth: false
    visible: GameModel.projectileInFlight
    x: GameModel.projectileX - width / 2
    y: GameModel.projectileY - height / 2
}
