import QtQuick

AnimatedSprite {
    id: explosion

    objectName: "explosion"
    source: "qrc:/ArtilleryDuel/assets/explosion_frames.png"
    frameWidth: 32
    frameHeight: 32
    frameCount: 4
    frameRate: 10
    loops: 1
    running: false
    visible: running
    width: 32
    height: 32
    smooth: false

    signal done

    onFinished: done()
}
