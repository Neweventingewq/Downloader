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

        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            AppLogo {
                Layout.preferredWidth: 56
                Layout.preferredHeight: 56
            }
            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true
                Text {
                    text: "Volchay Downloader"
                    color: theme.textPrimary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 22
                    font.weight: Font.Bold
                }
                Text {
                    text: "v0.1.0"
                    color: theme.textSecondary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 12
                }
            }
        }

        AcrylicCard {
            Layout.fillWidth: true
            Layout.preferredHeight: aboutCol.implicitHeight + 24
            ColumnLayout {
                id: aboutCol
                anchors.fill: parent
                anchors.margins: 14
                spacing: 10
                Text {
                    text: i18n.t("about.appDesc")
                    color: theme.textPrimary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        AcrylicCard {
            Layout.fillWidth: true
            Layout.preferredHeight: toolsCol.implicitHeight + 24
            ColumnLayout {
                id: toolsCol
                anchors.fill: parent
                anchors.margins: 14
                spacing: 8

                SectionHeader { text: i18n.t("about.tools") }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 3
                    columnSpacing: 12
                    rowSpacing: 6
                    Text { text: "yt-dlp"; color: theme.textPrimary; font.weight: Font.DemiBold; font.family: "Inter, Segoe UI, sans-serif"; font.pixelSize: 12 }
                    Text { text: tools.ytDlpVersion || (tools.ytDlpAvailable ? tools.ytDlpPath : i18n.t("about.notFound")); color: theme.textSecondary; font.family: "Inter, Segoe UI, sans-serif"; font.pixelSize: 11; elide: Text.ElideMiddle; Layout.fillWidth: true }
                    Text { text: tools.ytDlpPath; color: theme.textMuted; font.family: "Inter, Segoe UI, sans-serif"; font.pixelSize: 11; elide: Text.ElideMiddle; Layout.fillWidth: true }

                    Text { text: "ffmpeg"; color: theme.textPrimary; font.weight: Font.DemiBold; font.family: "Inter, Segoe UI, sans-serif"; font.pixelSize: 12 }
                    Text { text: tools.ffmpegVersion || (tools.ffmpegAvailable ? tools.ffmpegPath : i18n.t("about.notFound")); color: theme.textSecondary; font.family: "Inter, Segoe UI, sans-serif"; font.pixelSize: 11; elide: Text.ElideMiddle; Layout.fillWidth: true }
                    Text { text: tools.ffmpegPath; color: theme.textMuted; font.family: "Inter, Segoe UI, sans-serif"; font.pixelSize: 11; elide: Text.ElideMiddle; Layout.fillWidth: true }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
