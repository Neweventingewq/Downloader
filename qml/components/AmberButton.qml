import QtQuick
import QtQuick.Controls

// Primary call-to-action. Amber background + dark text. Hover / pressed
// states tint the accent.
Button {
    id: button
    text: ""
    implicitHeight: 38
    padding: 14
    hoverEnabled: true

    property color baseColor: theme.accent
    property color hoverColor: theme.accentHover
    property color pressColor: theme.accentDeep

    background: Rectangle {
        radius: 10
        color: button.pressed ? button.pressColor
                              : (button.hovered ? button.hoverColor : button.baseColor)
        Behavior on color { ColorAnimation { duration: 100 } }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 1
            height: 1
            color: Qt.rgba(1, 1, 1, 0.20)
            radius: 1
        }
    }

    contentItem: Text {
        text: button.text
        color: theme.accentTextOn
        font.family: "Inter, Segoe UI, sans-serif"
        font.pixelSize: 13
        font.weight: Font.DemiBold
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
