pragma Singleton
pragma ModuleName: "io.github.jandal.artillery-duel"

import QtQuick

QtObject {
    readonly property FontLoader fontLoader: FontLoader {
        source: "assets/fonts/c64_font.ttf"
    }

    readonly property string fontFamily: fontLoader.name

    readonly property color background: "#40318D"
    readonly property color chrome: "#5C47E4"
    readonly property color accent: "#BFCE72"
    readonly property color green: "#94E089"
    readonly property color red: "#B86962"
    readonly property color steel: "#9F9F9E"
    readonly property color dark: "#505050"
}
