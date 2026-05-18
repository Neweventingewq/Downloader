import QtQuick
import QtQuick.Shapes

// Tiny vector glyphs drawn with Shape — no SVG, no font dependency. The
// name maps to one of the hand-drawn shapes below.
Item {
    id: root
    width: 18
    height: 18

    property string iconName: "home"
    property color  color: "#FFFFFF"

    Shape {
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.samples: 4

        ShapePath {
            strokeColor: root.color
            fillColor: "transparent"
            strokeWidth: 1.7
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin

            PathSvg {
                path: {
                    switch (root.iconName) {
                    case "home":
                        // House
                        return "M3,9 L9,3 L15,9 L15,15 L11,15 L11,11 L7,11 L7,15 L3,15 Z"
                    case "queue":
                        // Three horizontal lines (list)
                        return "M3,5 H15 M3,9 H15 M3,13 H15"
                    case "history":
                        // Clock-back
                        return "M3.5,9 A5.5,5.5 0 1 0 6,4.5 M3.5,4.5 V8 H7 M9,7 V9.5 L11,11"
                    case "settings":
                        // Gear-ish (cross of strokes)
                        return "M9,3 V5 M9,13 V15 M3,9 H5 M13,9 H15 M5,5 L6.5,6.5 M11.5,11.5 L13,13 M5,13 L6.5,11.5 M11.5,6.5 L13,5"
                    case "about":
                        // Info "i"
                        return "M9,2.5 A6.5,6.5 0 1 1 9,15.5 A6.5,6.5 0 1 1 9,2.5 Z M9,6 V6.2 M9,8 V12"
                    case "video":
                        return "M3,5 H12 V13 H3 Z M12,8 L15,5 V13 L12,10 Z"
                    case "audio":
                        return "M6,4 V12 A2,2 0 0 1 4,14 A2,2 0 0 1 2,12 A2,2 0 0 1 4,10 H6 M6,4 H13 V11 A2,2 0 0 1 11,13 A2,2 0 0 1 9,11 A2,2 0 0 1 11,9 H13"
                    default:
                        return "M3,3 H15 V15 H3 Z"
                    }
                }
            }
        }
    }
}
