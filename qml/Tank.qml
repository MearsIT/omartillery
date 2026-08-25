
import QtQuick
import omartillery

Item {
    id: tank

    required property var player

    objectName: "tank_" + (player.facing < 0 ? "left" : "right")
    width: 16
    height: 16
    x: player.x - width / 2
    y: player.y - height
    opacity: player.alive ? 1.0 : 0.35

    Image {
        anchors.fill: parent
        source: tank.player.facing < 0
                ? "assets/tank_left.png"
                : "assets/tank_right.png"
        smooth: false
        fillMode: Image.Pad
    }

    Item {
        id: barrelPivot
        x: 8
        y: 8
        width: 1
        height: 1
        rotation: tank.player.facing < 0 ? tank.player.angle
                                         : -tank.player.angle

        Rectangle {
            width: 12
            height: 2
            radius: 0
            color: Theme.dark
            x: tank.player.facing < 0 ? -12 : 0
            y: -1
        }
    }
}
