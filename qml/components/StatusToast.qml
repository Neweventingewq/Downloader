import QtQuick

// Fade-in / fade-out toast.  Call show("message").
Rectangle {
    id: toast
    implicitHeight: 36
    implicitWidth: Math.min(label.implicitWidth + 28, 480)
    radius: 18
    color: theme.surface
    border.width: 1
    border.color: theme.border
    opacity: 0
    visible: opacity > 0.01

    function show(text) {
        label.text = text
        toast.opacity = 1
        hideTimer.restart()
    }

    Text {
        id: label
        anchors.centerIn: parent
        text: ""
        color: theme.textPrimary
        font.family: "Inter, Segoe UI, sans-serif"
        font.pixelSize: 12
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
    }

    Behavior on opacity { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }

    Timer {
        id: hideTimer
        interval: 1800
        onTriggered: toast.opacity = 0
    }
}
