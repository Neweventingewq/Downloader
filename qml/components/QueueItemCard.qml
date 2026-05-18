import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// One queue row.  Bound to a DownloadQueueModel row via the role names
// declared in DownloadQueueModel::roleNames().
AcrylicCard {
    id: card
    implicitHeight: column.implicitHeight + 24
    accentStripe: model.isActive
    Layout.fillWidth: true

    function humanBytes(bytes) {
        if (bytes < 0) return "—"
        const units = ["B", "KB", "MB", "GB", "TB"]
        let i = 0
        let n = bytes
        while (n >= 1024 && i < units.length - 1) { n /= 1024; ++i }
        return n.toFixed(n < 10 ? 1 : 0) + " " + units[i]
    }
    function humanSpeed(bps) {
        if (bps <= 0) return "—"
        return humanBytes(bps) + "/s"
    }
    function humanEta(sec) {
        if (sec < 0) return "—"
        const m = Math.floor(sec / 60), s = sec % 60
        if (m > 0) return m + ":" + (s < 10 ? "0" : "") + s
        return s + "s"
    }

    ColumnLayout {
        id: column
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            // Thumbnail placeholder square
            Rectangle {
                Layout.preferredWidth: 64
                Layout.preferredHeight: 40
                radius: 6
                color: theme.surfaceHover
                border.width: 1
                border.color: theme.border
                clip: true
                Text {
                    anchors.centerIn: parent
                    text: "▶"
                    color: theme.accent
                    font.pixelSize: 18
                }
            }

            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    text: model.title || model.url
                    color: theme.textPrimary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                    elide: Text.ElideRight
                }
                Text {
                    Layout.fillWidth: true
                    text: (model.uploader ? (model.uploader + " · ") : "")
                          + i18n.t(model.statusText || "status.queued")
                          + (model.errorText ? " — " + model.errorText : "")
                    color: theme.textSecondary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
            }

            // Per-row action buttons
            RowLayout {
                spacing: 6
                GhostButton {
                    visible: model.isActive
                    text: i18n.t("queue.cancel")
                    implicitHeight: 30
                    padding: 8
                    onClicked: manager.cancelJob(model.jobId)
                }
                GhostButton {
                    visible: model.status === 5 /* Failed */ || model.status === 6 /* Canceled */
                    text: i18n.t("queue.retry")
                    implicitHeight: 30
                    padding: 8
                    onClicked: manager.retryJob(model.jobId)
                }
                GhostButton {
                    visible: model.status === 4 /* Finished */ && model.outputFile !== ""
                    text: i18n.t("queue.openFile")
                    implicitHeight: 30
                    padding: 8
                    onClicked: manager.revealInFileManager(model.outputFile)
                }
                GhostButton {
                    visible: model.isTerminal
                    text: i18n.t("queue.remove")
                    implicitHeight: 30
                    padding: 8
                    onClicked: manager.removeJob(model.jobId)
                }
            }
        }

        // Progress + numbers
        ProgressBar {
            Layout.fillWidth: true
            value: model.progress
            indeterminate: model.status === 1 /* Resolving */ || model.status === 3 /* PostProcessing */
        }

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: Math.floor(model.progress * 100) + "%"
                color: theme.textPrimary
                font.family: "Inter, Segoe UI, sans-serif"
                font.pixelSize: 11
                font.weight: Font.DemiBold
            }
            Text {
                text: card.humanBytes(model.downloadedBytes)
                      + (model.totalBytes > 0 ? (" / " + card.humanBytes(model.totalBytes)) : "")
                color: theme.textSecondary
                font.family: "Inter, Segoe UI, sans-serif"
                font.pixelSize: 11
            }
            Item { Layout.fillWidth: true }
            Text {
                text: card.humanSpeed(model.speedBps)
                color: theme.textSecondary
                font.family: "Inter, Segoe UI, sans-serif"
                font.pixelSize: 11
            }
            Text {
                text: "ETA " + card.humanEta(model.etaSec)
                color: theme.textMuted
                font.family: "Inter, Segoe UI, sans-serif"
                font.pixelSize: 11
            }
        }
    }
}
