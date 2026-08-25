pragma ModuleName: "omartillery"

import QtQuick
import QtQuick.Controls
import omartillery

Button {
    id: control

    property int panelWidth: 230

    background: Rectangle {
        implicitWidth: control.panelWidth
        implicitHeight: 44
        color: control.pressed ? Theme.chrome : Theme.background
        border.color: control.enabled
                       ? (control.hovered ? Theme.accent : Theme.green)
                       : Theme.dark
        border.width: 3
    }

    contentItem: Text {
        text: control.text
        color: control.enabled ? Theme.accent : Theme.dark
        font.family: Theme.fontFamily
        font.pixelSize: 14
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
