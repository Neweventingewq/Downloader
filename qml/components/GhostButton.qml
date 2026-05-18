import QtQuick
import QtQuick.Controls

// Subtle button — no fill, 1px border in theme.border, hover lifts the
// background to theme.surfaceHover.  Used for secondary actions.
Button {
    id: button
    text: ""
    implicitHeight: 38
    padding: 12
    hoverEnabled: true

    background: Rectangle {
        radius: 10
        color: button.pressed ? theme.surfaceHover
                              : (button.hovered ? theme.surface : "transparent")
        border.width: 1
        border.color: theme.border
        Behavior on color { ColorAnimation { duration: 100 } }
    }

    contentItem: Text {
        text: button.text
        color: theme.textPrimary
        font.family: "Inter, Segoe UI, sans-serif"
        font.pixelSize: 13
        font.weight: Font.Medium
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
