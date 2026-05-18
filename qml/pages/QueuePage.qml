import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: page

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 12

        // Header + toolbar
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true
                Text {
                    text: i18n.t("nav.queue")
                    color: theme.textPrimary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 22
                    font.weight: Font.Bold
                }
                Text {
                    text: queue.activeCount + " · " + queue.queuedCount + " queued · "
                          + queue.finishedCount + " done · " + queue.failedCount + " failed"
                    color: theme.textSecondary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 12
                }
            }
            GhostButton {
                text: i18n.t("queue.clearDone")
                onClicked: queue.clearFinished()
            }
            GhostButton {
                text: i18n.t("queue.clearFailed")
                onClicked: queue.clearFailed()
            }
        }

        // List
        ScrollView {
            id: scroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: list
                width: scroll.availableWidth
                model: queue
                spacing: 10

                delegate: QueueItemCard {
                    width: ListView.view.width
                }

                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
            }
        }

        // Empty state
        Item {
            visible: queue.totalCount === 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 4
                Text {
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                    text: i18n.t("queue.empty")
                    color: theme.textPrimary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                }
                Text {
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                    text: i18n.t("queue.emptyHint")
                    color: theme.textSecondary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 12
                }
            }
        }
    }
}
