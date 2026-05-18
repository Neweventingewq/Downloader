import QtQuick

// The frosted-glass-ish base layer used by Main.qml. Renders a vertical
// gradient floor with three soft amber "lamps" — one near the top-right
// (window chrome glow) and two lower-left (content area glow). On the
// snow theme the lamps are muted so the white background stays clean;
// on blackout they're tightened down so the OLED look is preserved.
Item {
    id: root
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: theme.bgGradTop }
            GradientStop { position: 1.0; color: theme.bgGradBottom }
        }
    }

    // Top-right amber lamp
    Rectangle {
        x: parent.width - 360
        y: -120
        width: 480
        height: 480
        radius: width / 2
        color: theme.accent
        opacity: theme.snow ? 0.05 : (theme.blackout ? 0.06 : 0.10)
    }
    // Lower-left amber lamp
    Rectangle {
        x: -220
        y: parent.height - 280
        width: 520
        height: 520
        radius: width / 2
        color: theme.accentDeep
        opacity: theme.snow ? 0.04 : (theme.blackout ? 0.04 : 0.08)
    }
    // Mid-center subtle lamp
    Rectangle {
        x: parent.width / 2 - 120
        y: parent.height / 2 - 60
        width: 320
        height: 320
        radius: width / 2
        color: theme.accent
        opacity: theme.snow ? 0.02 : (theme.blackout ? 0.025 : 0.05)
    }

    // Subtle linear veil so the lamps fade gracefully into the floor.
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, theme.blackout ? 0.55 : 0.25) }
            GradientStop { position: 1.0; color: Qt.rgba(0, 0, 0, theme.blackout ? 0.65 : 0.20) }
        }
        opacity: theme.snow ? 0.0 : 1.0
    }
}
