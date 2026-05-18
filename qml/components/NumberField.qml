import QtQuick
import QtQuick.Controls

// Numeric text field with min/max clamping.
TextField {
    id: nf
    implicitHeight: 38
    color: theme.textPrimary
    selectionColor: theme.accent
    selectedTextColor: theme.accentTextOn
    placeholderTextColor: theme.textMuted
    font.family: "Inter, Segoe UI, sans-serif"
    font.pixelSize: 13
    leftPadding: 12
    rightPadding: 12
    horizontalAlignment: Text.AlignHCenter
    inputMethodHints: Qt.ImhDigitsOnly
    validator: IntValidator { bottom: nf.minimum; top: nf.maximum }

    property int minimum: 0
    property int maximum: 99
    property int value: 0

    onTextEdited: {
        const v = parseInt(text || "0", 10)
        if (!isNaN(v)) {
            value = Math.max(minimum, Math.min(maximum, v))
        }
    }

    Binding on text {
        value: String(nf.value)
        when: !nf.activeFocus
    }

    background: Rectangle {
        radius: 10
        color: theme.surface
        border.width: nf.activeFocus ? 1.5 : 1
        border.color: nf.activeFocus ? theme.accent : theme.border
    }
}
