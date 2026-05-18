import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Custom title bar (the window is frameless).  The empty area is a
// drag handle that delegates to startSystemMove() so the OS handles the
// move.  Three trafficlight buttons on the right do the usual things.
Item {
    id: bar
    height: 38

    signal minimizeRequested
    signal maximizeRequested
    signal closeRequested

    Rectangle {
        anchors.fill: parent
        color: theme.snow ? Qt.rgba(0, 0, 0, 0.02) : Qt.rgba(1, 1, 1, 0.02)
    }

    // Drag handle (everything that isn't a control)
    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.SizeAllCursor
        property point startPos
        property bool nativeMove: false
        onPressed: function(mouse) {
            nativeMove = false
            const win = bar.Window.window
            if (win && typeof win.startSystemMove === "function") {
                nativeMove = win.startSystemMove()
            }
            if (!nativeMove && win) {
                startPos = win.mapToGlobal(mouse.x, mouse.y)
            }
        }
        onPositionChanged: function(mouse) {
            if (pressed && !nativeMove) {
                const win = bar.Window.window
                if (!win) return
                const cur = win.mapToGlobal(mouse.x, mouse.y)
                win.x += (cur.x - startPos.x)
                win.y += (cur.y - startPos.y)
                startPos = cur
            }
        }
        onDoubleClicked: bar.maximizeRequested()
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 14
        anchors.rightMargin: 8
        spacing: 8

        AppLogo {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 22
            Layout.preferredHeight: 22
        }
        Text {
            text: "Volchay Downloader"
            color: theme.textPrimary
            font.family: "Inter, Segoe UI, sans-serif"
            font.pixelSize: 13
            font.weight: Font.DemiBold
        }
        Item { Layout.fillWidth: true }

        // Window buttons
        Repeater {
            model: [
                { glyph: "\u2014", action: "min" },   // —
                { glyph: "\u25A1", action: "max" },   // □
                { glyph: "\u2715", action: "close" }  // ✕
            ]
            Rectangle {
                width: 36; height: 28
                radius: 6
                color: hover.containsMouse
                       ? (modelData.action === "close" ? Qt.rgba(0.96, 0.25, 0.36, 0.85)
                                                       : theme.surfaceHover)
                       : "transparent"
                Behavior on color { ColorAnimation { duration: 100 } }
                Text {
                    anchors.centerIn: parent
                    text: modelData.glyph
                    color: (hover.containsMouse && modelData.action === "close")
                           ? "#FFFFFF" : theme.textSecondary
                    font.pixelSize: 13
                }
                MouseArea {
                    id: hover
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (modelData.action === "min")   bar.minimizeRequested()
                        if (modelData.action === "max")   bar.maximizeRequested()
                        if (modelData.action === "close") bar.closeRequested()
                    }
                }
            }
        }
    }
}
