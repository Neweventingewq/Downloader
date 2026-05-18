import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: page
    signal navigate(string target)

    // Per-session overrides; the Settings page edits the persistent
    // defaults, but the user can pick a one-off format here without
    // touching the global default.
    property string sessionFormat: settings.defaultFormat
    property string sessionOutputDir: settings.outputDir

    function fmtLabel(id) { return i18n.t("fmt." + id) }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 14

        // Header
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            Text {
                text: i18n.t("home.title")
                color: theme.textPrimary
                font.family: "Inter, Segoe UI, sans-serif"
                font.pixelSize: 26
                font.weight: Font.Bold
            }
            Text {
                text: i18n.t("home.subtitle")
                color: theme.textSecondary
                font.family: "Inter, Segoe UI, sans-serif"
                font.pixelSize: 13
            }
        }

        // Main card: URL + Format + Quality + Output dir
        AcrylicCard {
            Layout.fillWidth: true
            Layout.preferredHeight: form.implicitHeight + 28

            ColumnLayout {
                id: form
                anchors.fill: parent
                anchors.margins: 14
                spacing: 10

                // URL row
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    TextFieldA {
                        id: urlField
                        Layout.fillWidth: true
                        placeholderText: i18n.t("home.urlPlaceholder")
                        Keys.onReturnPressed: downloadButton.clicked()
                        Keys.onEnterPressed: downloadButton.clicked()
                    }
                    GhostButton {
                        text: i18n.t("home.paste")
                        onClicked: urlField.paste()
                    }
                    GhostButton {
                        text: i18n.t("home.clear")
                        onClicked: urlField.text = ""
                    }
                }

                // Format / quality row
                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 12
                    rowSpacing: 8

                    Text {
                        text: i18n.t("home.quality")
                        color: theme.textSecondary
                        font.family: "Inter, Segoe UI, sans-serif"
                        font.pixelSize: 12
                    }
                    ComboField {
                        id: qualityCombo
                        Layout.fillWidth: true
                        model: settings.formatChoices()
                        textRole: ""
                        delegate: ItemDelegate {
                            width: qualityCombo.width
                            contentItem: Text {
                                text: page.fmtLabel(modelData)
                                color: theme.textPrimary
                                font: qualityCombo.font
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: hovered ? theme.surfaceHover : "transparent"
                                radius: 6
                            }
                        }
                        displayText: page.fmtLabel(page.sessionFormat)
                        currentIndex: settings.formatChoices().indexOf(page.sessionFormat)
                        onActivated: function(idx) {
                            page.sessionFormat = settings.formatChoices()[idx]
                        }
                    }

                    Text {
                        text: i18n.t("home.outputDir")
                        color: theme.textSecondary
                        font.family: "Inter, Segoe UI, sans-serif"
                        font.pixelSize: 12
                    }
                    PathField {
                        Layout.fillWidth: true
                        path: page.sessionOutputDir
                        browseTitle: i18n.t("set.outputDir")
                        onPathPicked: function(p) {
                            page.sessionOutputDir = p
                            settings.outputDir = p
                        }
                    }
                }

                // Action row
                RowLayout {
                    Layout.fillWidth: true
                    Layout.topMargin: 6
                    spacing: 8
                    GhostButton {
                        text: i18n.t("home.openFolder")
                        onClicked: manager.openOutputDir()
                    }
                    Item { Layout.fillWidth: true }
                    GhostButton {
                        text: i18n.t("home.addToQueue")
                        enabled: manager.looksLikeSupportedUrl(urlField.text)
                        onClicked: {
                            manager.enqueueWithFormat(urlField.text, page.sessionFormat)
                            urlField.text = ""
                        }
                    }
                    AmberButton {
                        id: downloadButton
                        text: i18n.t("home.download")
                        enabled: manager.looksLikeSupportedUrl(urlField.text)
                                 && manager.canStart
                        onClicked: {
                            const id = manager.enqueueWithFormat(urlField.text, page.sessionFormat)
                            if (id > 0) {
                                urlField.text = ""
                                page.navigate("queue")
                            }
                        }
                    }
                }
            }
        }

        // Hint cards: tool status / active downloads
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 78
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 4
                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: "yt-dlp"
                            color: theme.textPrimary
                            font.family: "Inter, Segoe UI, sans-serif"
                            font.pixelSize: 12
                            font.weight: Font.DemiBold
                        }
                        Item { Layout.fillWidth: true }
                        Rectangle {
                            width: 8; height: 8; radius: 4
                            color: tools.ytDlpAvailable ? theme.success : theme.error
                        }
                    }
                    Text {
                        text: tools.ytDlpAvailable ? (tools.ytDlpVersion || tools.ytDlpPath)
                                                   : i18n.t("about.notFound")
                        color: theme.textSecondary
                        font.family: "Inter, Segoe UI, sans-serif"
                        font.pixelSize: 11
                        elide: Text.ElideMiddle
                        Layout.fillWidth: true
                    }
                }
            }

            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 78
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 4
                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: "ffmpeg"
                            color: theme.textPrimary
                            font.family: "Inter, Segoe UI, sans-serif"
                            font.pixelSize: 12
                            font.weight: Font.DemiBold
                        }
                        Item { Layout.fillWidth: true }
                        Rectangle {
                            width: 8; height: 8; radius: 4
                            color: tools.ffmpegAvailable ? theme.success : theme.warning
                        }
                    }
                    Text {
                        text: tools.ffmpegAvailable ? (tools.ffmpegVersion || tools.ffmpegPath)
                                                    : i18n.t("about.notFound")
                        color: theme.textSecondary
                        font.family: "Inter, Segoe UI, sans-serif"
                        font.pixelSize: 11
                        elide: Text.ElideMiddle
                        Layout.fillWidth: true
                    }
                }
            }

            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 78
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 4
                    Text {
                        text: i18n.t("nav.queue")
                        color: theme.textPrimary
                        font.family: "Inter, Segoe UI, sans-serif"
                        font.pixelSize: 12
                        font.weight: Font.DemiBold
                    }
                    Text {
                        text: queue.activeCount + " · " + queue.queuedCount + " · " + queue.finishedCount
                        color: theme.textSecondary
                        font.family: "Inter, Segoe UI, sans-serif"
                        font.pixelSize: 11
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: page.navigate("queue")
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
