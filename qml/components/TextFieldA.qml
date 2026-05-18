import QtQuick
import QtQuick.Controls

// Themed TextField — flat rectangle, 1px border, amber border on focus.
TextField {
    id: tf
    implicitHeight: 38
    color: theme.textPrimary
    selectionColor: theme.accent
    selectedTextColor: theme.accentTextOn
    placeholderTextColor: theme.textMuted
    font.family: "Inter, Segoe UI, sans-serif"
    font.pixelSize: 13
    leftPadding: 12
    rightPadding: 12

    background: Rectangle {
        radius: 10
        color: theme.surface
        border.width: tf.activeFocus ? 1.5 : 1
        border.color: tf.activeFocus ? theme.accent : theme.border
    }
}
