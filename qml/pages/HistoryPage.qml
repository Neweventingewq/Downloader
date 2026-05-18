import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

// History is just the queue model filtered to terminal rows.  We reuse
// the same QueueItemCard delegate but only render finished / failed /
// canceled rows.
Item {
    id: page

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true
                Text {
                    text: i18n.t("nav.history")
                    color: theme.textPrimary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 22
                    font.weight: Font.Bold
                }
                Text {
                    text: queue.finishedCount + " · " + queue.failedCount
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

        ScrollView {
            id: scroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                width: scroll.availableWidth
                model: queue
                spacing: 10

                delegate: Loader {
                    width: ListView.view.width
                    height: model.isTerminal ? (active && item ? item.implicitHeight : 0) : 0
                    visible: model.isTerminal
                    active: model.isTerminal
                    sourceComponent: QueueItemCard {}
                    onLoaded: {
                        if (item) {
                            for (const k of [
                                "jobId","url","title","uploader","thumbnailUrl","durationMs",
                                "totalBytes","downloadedBytes","speedBps","etaSec","progress",
                                "status","statusText","outputFile","outputDir","formatChoice",
                                "errorText","errorKey","startedAt","finishedAt","logTail","isActive","isTerminal"
                            ]) {
                                item.model = model
                            }
                        }
                    }
                }
            }
        }
    }
}
