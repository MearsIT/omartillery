import QtQuick
import Quickshell
import Quickshell.Ui

PanelItem {
    id: barWidget

    property string moduleId: "omartillery"

    property bool opened: panelLoader.item?.visible ?? false
    property bool popoutSwitchClosing: panelLoader.item?.popoutSwitchClosing ?? false

    implicitWidth: button.width
    implicitHeight: button.height

    function open() {
        if (panelLoader.item) {
            panelLoader.item.open();
        }
    }

    function close() {
        if (panelLoader.item) {
            panelLoader.item.close();
        }
    }

    function toggle() {
        if (panelLoader.item) {
            panelLoader.item.toggle();
        }
    }

    function closeForPopoutSwitch() {
        if (panelLoader.item) {
            panelLoader.item.closeForPopoutSwitch();
        }
    }

    function injectPanel() {
        if (!panelLoader.item) return;

        panelLoader.item.barWidget = barWidget;
        panelLoader.item.button = button;
        panelLoader.item.bar = barWidget.bar;
        panelLoader.item.hostWidget = barWidget;
    }

    Loader {
        id: panelLoader
        active: true
        visible: false
        source: "Panel.qml"

        onLoaded: {
            injectPanel();
            Qt.callLater(injectPanel);
        }
    }

    WidgetButton {
        id: button
        contentItem: Item {
            implicitWidth: label.width + 16
            implicitHeight: label.height + 8

            Text {
                id: label
                anchors.centerIn: parent
                text: "🎮"
                font.pixelSize: 16
                color: button.hovering ? "#BFCE72" : "#9F9F9E"
            }
        }

        tooltip: "Artillery Duel"

        onClicked: {
            toggle();
        }
    }
}
