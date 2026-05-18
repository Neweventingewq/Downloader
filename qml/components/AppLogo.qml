import QtQuick
import QtQuick.Shapes

// Brand mark — an amber "play" triangle inside a soft rounded square.
// Drawn entirely with QtQuick.Shapes so it renders crisp at any size
// and works without any image file.
Item {
    id: root
    implicitWidth: 36
    implicitHeight: 36

    Rectangle {
        anchors.fill: parent
        radius: parent.width * 0.22
        color: Qt.rgba(theme.accent.r, theme.accent.g, theme.accent.b, theme.snow ? 0.10 : 0.18)
        border.width: 1
        border.color: theme.accent
    }

    Shape {
        anchors.centerIn: parent
        width: parent.width * 0.45
        height: parent.height * 0.45
        antialiasing: true
        layer.enabled: true
        layer.samples: 4
        ShapePath {
            fillColor: theme.accent
            strokeColor: "transparent"
            startX: 0; startY: 0
            PathLine { x: 0;                          y: root.height * 0.45 }
            PathLine { x: root.width * 0.45 * 0.94;    y: root.height * 0.45 / 2 }
            PathLine { x: 0;                          y: 0 }
        }
    }
}
