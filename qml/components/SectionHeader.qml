import QtQuick
import QtQuick.Layouts   // needed for the Layout.* attached property below

// Small all-caps header used on the Settings page to label groups of
// related rows.
Item {
    id: root
    implicitHeight: label.implicitHeight + 8
    Layout.fillWidth: true  // ignored when used outside a layout

    property string text: ""

    Text {
        id: label
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        text: root.text.toUpperCase()
        color: theme.textMuted
        font.family: "Inter, Segoe UI, sans-serif"
        font.pixelSize: 10
        font.weight: Font.Bold
        font.letterSpacing: 1.4
        elide: Text.ElideRight
    }
}
