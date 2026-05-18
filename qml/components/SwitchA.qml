import QtQuick
import QtQuick.Controls

// Compact themed switch — amber when on, surface when off.
Switch {
    id: sw
    implicitHeight: 22
    implicitWidth: 40

    indicator: Rectangle {
        implicitWidth: 38
        implicitHeight: 20
        x: sw.leftPadding
        y: parent.height / 2 - height / 2
        radius: 10
        color: sw.checked ? theme.accent : theme.surface
        border.color: sw.checked ? theme.accent : theme.border
        border.width: 1
        Behavior on color { ColorAnimation { duration: 120 } }

        Rectangle {
            x: sw.checked ? parent.width - width - 2 : 2
            y: 2
            width: 16; height: 16
            radius: 8
            color: sw.checked ? theme.accentTextOn : theme.textSecondary
            Behavior on x { NumberAnimation { duration: 140; easing.type: Easing.OutCubic } }
        }
    }

    contentItem: Item {}
}
