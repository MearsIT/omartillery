import QtQuick
import Quickshell
import qs.Ui

BarWidget {
  id: root
  moduleName: "omartillery"

  WidgetButton {
    id: button
    bar: root.bar
    text: "ART"
    tooltipText: "Artillery Test"
    onPressed: function(buttonCode) {
      console.log("Artillery button pressed!")
    }
  }
}
