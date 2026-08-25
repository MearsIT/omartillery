import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root

    title: qsTr("Artillery Duel")
    width: 800
    height: 600
    minimumWidth: 640
    minimumHeight: 480
    visible: true
    color: "#40318D"

    GameBoard {
        anchors.centerIn: parent
        scale: Math.min(root.width / boardWidth, root.height / boardHeight)
    }
}
