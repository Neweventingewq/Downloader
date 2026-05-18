import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import "components"
import "pages"

ApplicationWindow {
    id: window
    visible: true
    width: theme.compact ? 480 : 1180
    height: theme.compact ? 720 : 740
    minimumWidth: theme.compact ? 380 : 920
    minimumHeight: theme.compact ? 560 : 600
    title: "Volchay Downloader"
    color: theme.bg
    flags: Qt.Window | Qt.FramelessWindowHint

    property string currentPage: "home"

    function applyCompactSize() {
        if (window.visibility === Window.Maximized
            || window.visibility === Window.FullScreen) return
        if (theme.compact) {
            window.width  = 480
            window.height = 720
        } else {
            window.width  = 1180
            window.height = 740
        }
    }
    Connections {
        target: theme
        function onCompactChanged() { window.applyCompactSize() }
    }

    AcrylicBackground { id: backdrop }

    // 1px highlight rim around the whole window — gives the frameless
    // window a clean edge on every theme.
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.width: 1
        border.color: theme.divider
        radius: 0
        z: 1000
    }

    TitleBar {
        id: titleBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        z: 5
        onMinimizeRequested: window.showMinimized()
        onMaximizeRequested: window.visibility === Window.Maximized
                              ? window.showNormal() : window.showMaximized()
        onCloseRequested: window.close()
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: titleBar.bottom
        height: 1
        color: theme.divider
        z: 5
    }

    RowLayout {
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 1
        spacing: 0

        // Sidebar
        Item {
            id: sidebarWrap
            Layout.preferredWidth: theme.compact ? 64 : 232
            Layout.fillHeight: true
            Layout.topMargin: 10
            Layout.bottomMargin: 12
            Layout.leftMargin: 10
            Layout.rightMargin: 6

            Behavior on Layout.preferredWidth {
                NumberAnimation { duration: 180; easing.type: Easing.OutCubic }
            }

            Rectangle {
                anchors.fill: parent
                radius: 14
                color: theme.sidebarBg
                border.width: 1
                border.color: theme.border

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 1
                    height: 1
                    radius: 1
                    color: theme.snow ? Qt.rgba(0, 0, 0, 0.05)
                                      : Qt.rgba(1, 1, 1, 0.05)
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.topMargin: 14
                    anchors.bottomMargin: 10
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    spacing: 0

                    Text {
                        Layout.leftMargin: 18
                        Layout.bottomMargin: 8
                        Layout.fillWidth: true
                        text: i18n.t("nav.home").toUpperCase()
                        color: theme.textMuted
                        font.family: "Inter, Segoe UI, sans-serif"
                        font.pixelSize: 10
                        font.weight: Font.Bold
                        font.letterSpacing: 1.4
                        visible: !theme.compact
                        elide: Text.ElideRight
                    }

                    SidebarItem {
                        Layout.fillWidth: true
                        iconName: "home"
                        text: i18n.t("nav.home")
                        compact: theme.compact
                        active: window.currentPage === "home"
                        onClicked: window.currentPage = "home"
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        iconName: "queue"
                        text: i18n.t("nav.queue")
                        compact: theme.compact
                        active: window.currentPage === "queue"
                        badgeText: queue.activeCount + queue.queuedCount > 0
                                   ? String(queue.activeCount + queue.queuedCount) : ""
                        onClicked: window.currentPage = "queue"
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        iconName: "history"
                        text: i18n.t("nav.history")
                        compact: theme.compact
                        active: window.currentPage === "history"
                        onClicked: window.currentPage = "history"
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        iconName: "settings"
                        text: i18n.t("nav.settings")
                        compact: theme.compact
                        active: window.currentPage === "settings"
                        onClicked: window.currentPage = "settings"
                    }
                    SidebarItem {
                        Layout.fillWidth: true
                        iconName: "about"
                        text: i18n.t("nav.about")
                        compact: theme.compact
                        active: window.currentPage === "about"
                        onClicked: window.currentPage = "about"
                    }

                    Item { Layout.fillHeight: true }

                    // Footer chip with logo + version
                    Item {
                        Layout.fillWidth: true
                        Layout.bottomMargin: 10
                        Layout.leftMargin: theme.compact ? 8 : 12
                        Layout.rightMargin: theme.compact ? 8 : 12
                        height: theme.compact ? 44 : 56
                        Rectangle {
                            anchors.fill: parent
                            radius: 10
                            color: theme.surface
                            border.width: 1
                            border.color: theme.border
                        }
                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: theme.compact ? 6 : 12
                            anchors.rightMargin: theme.compact ? 6 : 12
                            spacing: 10
                            AppLogo {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.preferredWidth: theme.compact ? 28 : 36
                                Layout.preferredHeight: theme.compact ? 28 : 36
                            }
                            ColumnLayout {
                                spacing: 0
                                Layout.fillWidth: true
                                visible: !theme.compact
                                Text {
                                    text: "Volchay Downloader"
                                    color: theme.textPrimary
                                    font.family: "Inter, Segoe UI, sans-serif"
                                    font.pixelSize: 12
                                    font.weight: Font.DemiBold
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }
                                Text {
                                    text: "v0.1.0"
                                    color: theme.textMuted
                                    font.family: "Inter, Segoe UI, sans-serif"
                                    font.pixelSize: 10
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                }
            }
        }

        // Content
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 10
            Layout.bottomMargin: 12
            Layout.rightMargin: 12

            StackLayout {
                id: stack
                anchors.fill: parent
                currentIndex: switch (window.currentPage) {
                              case "home":     return 0
                              case "queue":    return 1
                              case "history":  return 2
                              case "settings": return 3
                              case "about":    return 4
                              }

                HomePage    { onNavigate: function(target) { window.currentPage = target } }
                QueuePage   {}
                HistoryPage {}
                SettingsPage{}
                AboutPage   {}
            }
        }
    }

    // Toast (appears for ~1.6s)
    StatusToast {
        id: toast
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter
        z: 9999
    }

    Connections {
        target: manager
        function onToastEmitted() {
            toast.show(i18n.t(manager.lastToastKey))
        }
    }

    // Resize handle (bottom-right). Delegates to startSystemResize() so
    // the OS compositor handles the live resize.
    MouseArea {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: 16; height: 16
        cursorShape: Qt.SizeFDiagCursor
        property point startPos
        property size startSize
        property bool nativeResize: false
        onPressed: function(mouse) {
            nativeResize = false
            if (typeof window.startSystemResize === "function") {
                nativeResize = window.startSystemResize(Qt.BottomEdge | Qt.RightEdge)
            }
            if (!nativeResize) {
                startPos = window.mapToGlobal(mouse.x, mouse.y)
                startSize = Qt.size(window.width, window.height)
            }
        }
        onPositionChanged: function(mouse) {
            if (pressed && !nativeResize) {
                const cur = window.mapToGlobal(mouse.x, mouse.y)
                window.width = Math.max(window.minimumWidth, startSize.width + (cur.x - startPos.x))
                window.height = Math.max(window.minimumHeight, startSize.height + (cur.y - startPos.y))
            }
        }
    }
}
