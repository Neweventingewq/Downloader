import QtQuick

// Slim 4-px progress bar with amber fill.  `value` is 0..1.
Item {
    id: root
    implicitHeight: 4
    property real value: 0
    property bool indeterminate: false

    Rectangle {
        anchors.fill: parent
        radius: 2
        color: theme.surface
        border.width: 1
        border.color: theme.border
    }

    Rectangle {
        height: parent.height
        radius: 2
        color: theme.accent
        width: Math.max(0, Math.min(parent.width, parent.width * root.value))
        visible: !root.indeterminate
        Behavior on width { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
    }

    // Indeterminate shuttle
    Rectangle {
        visible: root.indeterminate
        width: parent.width * 0.35
        height: parent.height
        radius: 2
        color: theme.accent
        SequentialAnimation on x {
            running: root.indeterminate
            loops: Animation.Infinite
            NumberAnimation { from: -root.width * 0.35; to: root.width; duration: 1200; easing.type: Easing.InOutQuad }
        }
    }
}
