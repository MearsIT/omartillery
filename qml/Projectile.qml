pragma ModuleName: "io.github.jandal.artillery-duel"

import QtQuick
import io.github.jandal.artillery-duel

Image {
    id: projectile

    objectName: "projectile"
    source: "assets/projectile.png"
    width: 8
    height: 8
    smooth: false
    visible: GameEngine.projectileInFlight
    x: GameEngine.projectileX - width / 2
    y: GameEngine.projectileY - height / 2
}
