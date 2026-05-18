import QtQuick
import QtQuick.Layouts

// One row in the sidebar.  When `active`, the row gets an amber left
// stripe + soft accent-tinted background — same idea as the polished
// Luna sidebar, but adapted to the Acri amber palette.
Item {
    id: row
    height: compact ? 44 : 40

    property string iconName: ""
    property string text: ""
    property bool   active: false
    property bool   compact: false
    property string badgeText: ""

    signal clicked

    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        anchors.topMargin: 2
        anchors.bottomMargin: 2
        radius: 10
        color: row.active
               ? theme.accentSoft
               : (mouse.containsMouse ? theme.surfaceHover : "transparent")
        Behavior on color { ColorAnimation { duration: 120 } }

        // Active accent stripe
        Rectangle {
            visible: row.active
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6
            width: 3
            radius: 2
            color: theme.accent
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: row.compact ? 0 : 16
            anchors.rightMargin: row.compact ? 0 : 12
            spacing: 12

            // Icon
            SidebarIcon {
                Layout.alignment: row.compact ? Qt.AlignHCenter : Qt.AlignVCenter
                Layout.preferredWidth: row.compact ? 44 : 18
                Layout.preferredHeight: 18
                iconName: row.iconName
                color: row.active ? theme.accent : theme.textSecondary
            }

            Text {
                visible: !row.compact
                text: row.text
                color: row.active ? theme.textPrimary : theme.textSecondary
                font.family: "Inter, Segoe UI, sans-serif"
                font.pixelSize: 13
                font.weight: row.active ? Font.DemiBold : Font.Normal
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            // Badge (queue count)
            Rectangle {
                visible: !row.compact && row.badgeText !== ""
                Layout.preferredWidth: badge.implicitWidth + 12
                Layout.preferredHeight: 18
                radius: 9
                color: theme.accent
                Text {
                    id: badge
                    anchors.centerIn: parent
                    text: row.badgeText
                    color: theme.accentTextOn
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 10
                    font.weight: Font.Bold
                }
            }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: row.clicked()
        }
    }
}
