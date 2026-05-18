import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

// Wide flexible-configuration page.  Every control is bound directly to
// a Settings property so changes propagate (and persist) immediately.
Item {
    id: page

    function themeLabel(id) { return i18n.t("set.theme." + id) }
    function langLabel(id)  { return i18n.t("set.language." + id) }
    function fmtLabel(id)   { return i18n.t("fmt." + id) }

    ScrollView {
        anchors.fill: parent
        clip: true

        ColumnLayout {
            id: column
            width: page.width - 14
            spacing: 12
            anchors.leftMargin: 6
            anchors.rightMargin: 6

            // Header
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Text {
                    text: i18n.t("set.title")
                    color: theme.textPrimary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 22
                    font.weight: Font.Bold
                }
                Text {
                    text: i18n.t("set.subtitle")
                    color: theme.textSecondary
                    font.family: "Inter, Segoe UI, sans-serif"
                    font.pixelSize: 12
                }
            }

            // ===== Appearance =====
            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: themeGrid.implicitHeight + 24
                ColumnLayout {
                    id: themeGrid
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    SectionHeader { text: i18n.t("set.section.theme") }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 12
                        rowSpacing: 8

                        Text { text: i18n.t("set.theme"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6
                            Repeater {
                                model: settings.themeChoices()
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 36
                                    radius: 10
                                    color: settings.themeMode === modelData ? theme.accentSoft : theme.surface
                                    border.width: 1
                                    border.color: settings.themeMode === modelData ? theme.accent : theme.border
                                    Text {
                                        anchors.centerIn: parent
                                        text: page.themeLabel(modelData)
                                        color: theme.textPrimary
                                        font.pixelSize: 12
                                        font.family: "Inter, Segoe UI, sans-serif"
                                        font.weight: settings.themeMode === modelData ? Font.DemiBold : Font.Normal
                                    }
                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: settings.themeMode = modelData
                                    }
                                }
                            }
                        }

                        Text { text: i18n.t("set.compact"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA {
                            checked: settings.compact
                            onCheckedChanged: if (checked !== settings.compact) settings.compact = checked
                        }

                        Text { text: i18n.t("set.language"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        ComboField {
                            Layout.preferredWidth: 220
                            model: settings.languageChoices()
                            displayText: page.langLabel(settings.language)
                            currentIndex: settings.languageChoices().indexOf(settings.language)
                            onActivated: function(idx) {
                                settings.language = settings.languageChoices()[idx]
                            }
                            delegate: ItemDelegate {
                                width: parent ? parent.width : 220
                                contentItem: Text {
                                    text: page.langLabel(modelData)
                                    color: theme.textPrimary
                                    font.family: "Inter, Segoe UI, sans-serif"
                                    font.pixelSize: 13
                                    verticalAlignment: Text.AlignVCenter
                                }
                                background: Rectangle {
                                    color: hovered ? theme.surfaceHover : "transparent"
                                    radius: 6
                                }
                            }
                        }
                    }
                }
            }

            // ===== Output =====
            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: outputGrid.implicitHeight + 24
                ColumnLayout {
                    id: outputGrid
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    SectionHeader { text: i18n.t("set.section.output") }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 12
                        rowSpacing: 8

                        Text { text: i18n.t("set.outputDir"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        PathField {
                            Layout.fillWidth: true
                            path: settings.outputDir
                            browseTitle: i18n.t("set.outputDir")
                            onPathPicked: function(p) { settings.outputDir = p }
                        }

                        Text { text: i18n.t("set.filenameTemplate"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        TextFieldA {
                            Layout.fillWidth: true
                            text: settings.filenameTemplate
                            onEditingFinished: settings.filenameTemplate = text
                        }

                        Text { text: i18n.t("set.restrictFilenames"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA {
                            checked: settings.restrictFilenames
                            onCheckedChanged: if (checked !== settings.restrictFilenames) settings.restrictFilenames = checked
                        }

                        Text { text: i18n.t("set.organizeByUploader"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA {
                            checked: settings.organizeByUploader
                            onCheckedChanged: if (checked !== settings.organizeByUploader) settings.organizeByUploader = checked
                        }
                    }
                }
            }

            // ===== Format =====
            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: formatGrid.implicitHeight + 24
                ColumnLayout {
                    id: formatGrid
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    SectionHeader { text: i18n.t("set.section.format") }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 12
                        rowSpacing: 8

                        Text { text: i18n.t("set.defaultFormat"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        ComboField {
                            Layout.preferredWidth: 260
                            model: settings.formatChoices()
                            displayText: page.fmtLabel(settings.defaultFormat)
                            currentIndex: settings.formatChoices().indexOf(settings.defaultFormat)
                            onActivated: function(idx) { settings.defaultFormat = settings.formatChoices()[idx] }
                            delegate: ItemDelegate {
                                width: parent ? parent.width : 260
                                contentItem: Text {
                                    text: page.fmtLabel(modelData)
                                    color: theme.textPrimary
                                    font.family: "Inter, Segoe UI, sans-serif"
                                    font.pixelSize: 13
                                    verticalAlignment: Text.AlignVCenter
                                }
                                background: Rectangle { color: hovered ? theme.surfaceHover : "transparent"; radius: 6 }
                            }
                        }

                        Text { text: i18n.t("set.containerVideo"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        ComboField {
                            Layout.preferredWidth: 160
                            model: settings.containerVideoChoices()
                            currentIndex: settings.containerVideoChoices().indexOf(settings.containerVideo)
                            onActivated: function(idx) { settings.containerVideo = settings.containerVideoChoices()[idx] }
                        }

                        Text { text: i18n.t("set.audioFormat"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        ComboField {
                            Layout.preferredWidth: 160
                            model: settings.audioFormatChoices()
                            currentIndex: settings.audioFormatChoices().indexOf(settings.audioFormat)
                            onActivated: function(idx) { settings.audioFormat = settings.audioFormatChoices()[idx] }
                        }

                        Text { text: i18n.t("set.audioQuality"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        NumberField {
                            Layout.preferredWidth: 100
                            minimum: 0; maximum: 10
                            value: settings.audioQuality
                            onValueChanged: settings.audioQuality = value
                        }

                        Text { text: i18n.t("set.preferFreeFormats"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA {
                            checked: settings.preferFreeFormats
                            onCheckedChanged: if (checked !== settings.preferFreeFormats) settings.preferFreeFormats = checked
                        }
                    }
                }
            }

            // ===== Extras =====
            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: extrasGrid.implicitHeight + 24
                ColumnLayout {
                    id: extrasGrid
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    SectionHeader { text: i18n.t("set.section.extras") }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 12
                        rowSpacing: 8

                        Text { text: i18n.t("set.writeSubs"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA { checked: settings.writeSubs; onCheckedChanged: if (checked !== settings.writeSubs) settings.writeSubs = checked }

                        Text { text: i18n.t("set.writeAutoSubs"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA { checked: settings.writeAutoSubs; onCheckedChanged: if (checked !== settings.writeAutoSubs) settings.writeAutoSubs = checked }

                        Text { text: i18n.t("set.embedSubs"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA { checked: settings.embedSubs; onCheckedChanged: if (checked !== settings.embedSubs) settings.embedSubs = checked }

                        Text { text: i18n.t("set.subLangs"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        TextFieldA {
                            Layout.fillWidth: true
                            text: settings.subLangs
                            onEditingFinished: settings.subLangs = text
                        }

                        Text { text: i18n.t("set.writeThumbnail"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA { checked: settings.writeThumbnail; onCheckedChanged: if (checked !== settings.writeThumbnail) settings.writeThumbnail = checked }

                        Text { text: i18n.t("set.embedThumbnail"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA { checked: settings.embedThumbnail; onCheckedChanged: if (checked !== settings.embedThumbnail) settings.embedThumbnail = checked }

                        Text { text: i18n.t("set.embedMetadata"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA { checked: settings.embedMetadata; onCheckedChanged: if (checked !== settings.embedMetadata) settings.embedMetadata = checked }

                        Text { text: i18n.t("set.embedChapters"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA { checked: settings.embedChapters; onCheckedChanged: if (checked !== settings.embedChapters) settings.embedChapters = checked }

                        Text { text: i18n.t("set.sponsorblock"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        SwitchA { checked: settings.sponsorblock; onCheckedChanged: if (checked !== settings.sponsorblock) settings.sponsorblock = checked }
                    }
                }
            }

            // ===== Network =====
            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: netGrid.implicitHeight + 24
                ColumnLayout {
                    id: netGrid
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    SectionHeader { text: i18n.t("set.section.network") }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 12
                        rowSpacing: 8

                        Text { text: i18n.t("set.maxConcurrent"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        NumberField {
                            Layout.preferredWidth: 100
                            minimum: 1; maximum: 8
                            value: settings.maxConcurrent
                            onValueChanged: settings.maxConcurrent = value
                        }

                        Text { text: i18n.t("set.speedLimit"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        NumberField {
                            Layout.preferredWidth: 140
                            minimum: 0; maximum: 1000000
                            value: settings.speedLimitKBps
                            onValueChanged: settings.speedLimitKBps = value
                        }

                        Text { text: i18n.t("set.retries"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        NumberField {
                            Layout.preferredWidth: 100
                            minimum: 0; maximum: 100
                            value: settings.retries
                            onValueChanged: settings.retries = value
                        }

                        Text { text: i18n.t("set.proxy"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        TextFieldA {
                            Layout.fillWidth: true
                            placeholderText: "socks5://127.0.0.1:1080"
                            text: settings.proxyUrl
                            onEditingFinished: settings.proxyUrl = text
                        }

                        Text { text: i18n.t("set.cookiesBrowser"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        ComboField {
                            Layout.preferredWidth: 200
                            model: settings.cookiesBrowserChoices()
                            currentIndex: settings.cookiesBrowserChoices().indexOf(settings.cookiesFromBrowser)
                            onActivated: function(idx) { settings.cookiesFromBrowser = settings.cookiesBrowserChoices()[idx] }
                        }

                        Text { text: i18n.t("set.cookiesFile"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        TextFieldA {
                            Layout.fillWidth: true
                            text: settings.cookiesFile
                            onEditingFinished: settings.cookiesFile = text
                        }

                        Text { text: i18n.t("set.userAgent"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        TextFieldA {
                            Layout.fillWidth: true
                            text: settings.userAgent
                            onEditingFinished: settings.userAgent = text
                        }
                    }
                }
            }

            // ===== Advanced =====
            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: advGrid.implicitHeight + 24
                ColumnLayout {
                    id: advGrid
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    SectionHeader { text: i18n.t("set.section.advanced") }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 12
                        rowSpacing: 8

                        Text { text: i18n.t("set.ytDlpPath"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        TextFieldA {
                            Layout.fillWidth: true
                            placeholderText: "/usr/local/bin/yt-dlp"
                            text: settings.ytDlpPathOverride
                            onEditingFinished: settings.ytDlpPathOverride = text
                        }

                        Text { text: i18n.t("set.ffmpegPath"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        TextFieldA {
                            Layout.fillWidth: true
                            placeholderText: "/usr/bin/ffmpeg"
                            text: settings.ffmpegPathOverride
                            onEditingFinished: settings.ffmpegPathOverride = text
                        }

                        Text { text: i18n.t("set.extraArgs"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        TextFieldA {
                            Layout.fillWidth: true
                            placeholderText: "--throttled-rate 100K"
                            text: settings.extraArgs
                            onEditingFinished: settings.extraArgs = text
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.topMargin: 6
                        Item { Layout.fillWidth: true }
                        GhostButton {
                            text: i18n.t("set.reset")
                            onClicked: confirmReset.open()
                        }
                    }
                }
            }

            // Padding so the last card doesn't sit on the scroll edge.
            Item { Layout.preferredHeight: 8 }
        }
    }

    Dialog {
        id: confirmReset
        modal: true
        title: i18n.t("set.resetConfirm")
        standardButtons: Dialog.Yes | Dialog.No
        onAccepted: settings.resetToDefaults()
    }
}
