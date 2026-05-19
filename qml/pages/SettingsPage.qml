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
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            ComboField {
                                Layout.preferredWidth: 160
                                model: settings.containerVideoChoices()
                                currentIndex: settings.containerVideoChoices().indexOf(settings.containerVideo)
                                onActivated: function(idx) { settings.containerVideo = settings.containerVideoChoices()[idx] }
                            }
                            // Short blurb about container compatibility — we now
                            // prefer AAC audio inside MP4 specifically so the
                            // file plays in Windows' built-in Films & TV,
                            // worth telling the user the trade-off.
                            Text {
                                Layout.fillWidth: true
                                visible: settings.containerVideo === "mp4"
                                text: i18n.t("set.containerVideo.mp4.hint")
                                color: theme.textSecondary
                                opacity: 0.75
                                font.pixelSize: 11
                                font.family: "Inter, Segoe UI, sans-serif"
                                wrapMode: Text.WordWrap
                            }
                            Text {
                                Layout.fillWidth: true
                                visible: settings.containerVideo === "mkv" || settings.containerVideo === "webm"
                                text: i18n.t("set.containerVideo.other.hint")
                                color: theme.textSecondary
                                opacity: 0.75
                                font.pixelSize: 11
                                font.family: "Inter, Segoe UI, sans-serif"
                                wrapMode: Text.WordWrap
                            }
                        }

                        Text { text: i18n.t("set.audioFormat"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            ComboField {
                                Layout.preferredWidth: 160
                                model: settings.audioFormatChoices()
                                currentIndex: settings.audioFormatChoices().indexOf(settings.audioFormat)
                                onActivated: function(idx) { settings.audioFormat = settings.audioFormatChoices()[idx] }
                            }
                            // The mp3/m4a/opus picker only applies to audio-only
                            // extraction (the "Audio" quick action / playlist
                            // audio mode).  For video downloads yt-dlp keeps the
                            // best audio stream from YouTube as-is — surfacing
                            // that out loud avoids the "why is my MP4 still in
                            // opus?" surprise.
                            Text {
                                Layout.fillWidth: true
                                text: i18n.t("set.audioFormat.hint")
                                color: theme.textSecondary
                                opacity: 0.75
                                font.pixelSize: 11
                                font.family: "Inter, Segoe UI, sans-serif"
                                wrapMode: Text.WordWrap
                            }
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

            // ===== Sign-in =====
            // YouTube login lives in its own card because (a) it owns the
            // most important per-user state in the app (a real Google
            // session), and (b) the warning banner about secondary
            // accounts needs visual prominence.  Anything cookie-related
            // that doesn't involve a live in-app login (manual file
            // upload, cookies-from-other-browser) stays in the Network
            // section below.
            AcrylicCard {
                Layout.fillWidth: true
                Layout.preferredHeight: signInGrid.implicitHeight + 24
                // Refresh the cookie jar whenever the user opens the
                // Settings page — picks up any login state that might
                // have settled after a previous LoginWindow closed.
                Component.onCompleted: if (typeof cookieJar !== 'undefined') cookieJar.refresh()
                ColumnLayout {
                    id: signInGrid
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    SectionHeader { text: i18n.t("set.section.signin") }

                    // Non-dismissible warning banner.  Hardcoded amber
                    // background so it reads as "caution" in every
                    // theme, even if the surrounding card is dark.
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: warnText.implicitHeight + 22
                        radius: 8
                        color: "#3a2d05"
                        border.color: "#c79a23"
                        border.width: 1
                        Text {
                            id: warnText
                            anchors.fill: parent
                            anchors.margins: 11
                            text: i18n.t("signin.warning")
                            color: "#ffe7a4"
                            font.pixelSize: 12
                            font.family: "Inter, Segoe UI, sans-serif"
                            wrapMode: Text.WordWrap
                        }
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 12
                        rowSpacing: 8

                        Text {
                            text: i18n.t("signin.youtube")
                            color: theme.textSecondary
                            font.pixelSize: 12
                            font.family: "Inter, Segoe UI, sans-serif"
                        }
                        RowLayout {
                            spacing: 8
                            Layout.fillWidth: true

                            // Status pill.  Green when signed in, gray
                            // otherwise.  Matches the visual language
                            // already used by other status indicators
                            // (queue rows, toolbar badges).
                            Rectangle {
                                Layout.preferredHeight: 26
                                Layout.preferredWidth: statusLabel.implicitWidth + 18
                                radius: 13
                                color: cookieJar.youtubeLoggedIn ? "#1f3a1f" : theme.surface
                                border.color: cookieJar.youtubeLoggedIn ? "#5fb45f" : theme.border
                                border.width: 1
                                Text {
                                    id: statusLabel
                                    anchors.centerIn: parent
                                    text: cookieJar.youtubeLoggedIn
                                          ? i18n.t("signin.status.signedin")
                                          : i18n.t("signin.status.notsignedin")
                                    color: cookieJar.youtubeLoggedIn ? "#bcefbc" : theme.textSecondary
                                    font.pixelSize: 12
                                    font.family: "Inter, Segoe UI, sans-serif"
                                }
                            }

                            AmberButton {
                                text: cookieJar.youtubeLoggedIn
                                      ? i18n.t("signin.button.signinAgain")
                                      : i18n.t("signin.button.signin")
                                onClicked: loginWindow.openLogin()
                            }
                            GhostButton {
                                visible: cookieJar.youtubeLoggedIn
                                text: i18n.t("signin.button.signout")
                                onClicked: cookieJar.signOutYouTube()
                            }
                            Item { Layout.fillWidth: true }
                        }
                    }

                    // Diagnostic counter — shows how many cookies the
                    // jar actually tracks for Google/YouTube domains.
                    // Useful when the login-state heuristic disagrees
                    // with what the user just did inside LoginWindow:
                    // a non-zero count here means cookies are stored,
                    // we just didn't recognise an auth one yet.
                    Text {
                        Layout.fillWidth: true
                        visible: cookieJar.trackedCookieCount > 0
                        text: i18n.t("signin.cookieCount")
                              .replace("%1", cookieJar.trackedCookieCount)
                        color: theme.textSecondary
                        opacity: 0.65
                        font.pixelSize: 11
                        font.family: "Inter, Segoe UI, sans-serif"
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        Layout.fillWidth: true
                        text: i18n.t("signin.hint")
                        color: theme.textSecondary
                        opacity: 0.75
                        font.pixelSize: 11
                        font.family: "Inter, Segoe UI, sans-serif"
                        wrapMode: Text.WordWrap
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

                        // Polite-mode pause.  Translated to yt-dlp's
                        // --sleep-requests / --sleep-interval. Default 3 s
                        // is the sweet spot: low enough to feel instant,
                        // high enough to never trigger YouTube’s rate-
                        // limiter on a logged-in account.
                        Text { text: i18n.t("set.politeSleep"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            NumberField {
                                Layout.preferredWidth: 100
                                minimum: 0; maximum: 30
                                value: settings.politeSleepSec
                                onValueChanged: settings.politeSleepSec = value
                            }
                            Text {
                                Layout.fillWidth: true
                                text: i18n.t("set.politeSleep.hint")
                                color: theme.textSecondary
                                opacity: 0.75
                                font.pixelSize: 11
                                font.family: "Inter, Segoe UI, sans-serif"
                                wrapMode: Text.WordWrap
                            }
                        }

                        Text { text: i18n.t("set.proxy"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        TextFieldA {
                            Layout.fillWidth: true
                            placeholderText: "socks5://127.0.0.1:1080"
                            text: settings.proxyUrl
                            onEditingFinished: settings.proxyUrl = text
                        }

                        Text { text: i18n.t("set.cookiesBrowser"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            ComboField {
                                Layout.preferredWidth: 200
                                model: settings.cookiesBrowserChoices()
                                currentIndex: settings.cookiesBrowserChoices().indexOf(settings.cookiesFromBrowser)
                                onActivated: function(idx) { settings.cookiesFromBrowser = settings.cookiesBrowserChoices()[idx] }
                            }
                            // Surfaced inline, not as a tooltip, so the user sees it
                            // immediately when the dropdown is on a Chromium-based
                            // browser — that's where the database-lock issue bites.
                            Text {
                                Layout.fillWidth: true
                                visible: settings.cookiesFromBrowser !== "none"
                                      && settings.cookiesFromBrowser !== "firefox"
                                      && settings.cookiesFromBrowser !== "safari"
                                      && settings.cookiesFromBrowser !== ""
                                text: i18n.t("set.cookiesBrowser.hint")
                                color: theme.textSecondary
                                opacity: 0.75
                                font.pixelSize: 11
                                font.family: "Inter, Segoe UI, sans-serif"
                                wrapMode: Text.WordWrap
                            }
                        }

                        Text { text: i18n.t("set.cookiesFile"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        FilePickField {
                            Layout.fillWidth: true
                            path: settings.cookiesFile
                            browseTitle: i18n.t("set.cookiesFile")
                            // Netscape cookies.txt is the only sensible format here,
                            // but we keep `All files` as the second filter so users with
                            // unusual file names (no extension, .cookies, .dat…) still
                            // see their file in the picker.
                            nameFilters: [
                                "Netscape cookies (*.txt cookies.txt)",
                                "All files (*)"
                            ]
                            onFilePicked: function(p) { settings.cookiesFile = p }
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
                        FilePickField {
                            Layout.fillWidth: true
                            path: settings.ytDlpPathOverride
                            browseTitle: i18n.t("set.ytDlpPath")
                            nameFilters: [
                                "yt-dlp executable (yt-dlp yt-dlp.exe)",
                                "Executables (*.exe)",
                                "All files (*)"
                            ]
                            onFilePicked: function(p) { settings.ytDlpPathOverride = p }
                        }

                        Text { text: i18n.t("set.ffmpegPath"); color: theme.textSecondary; font.pixelSize: 12; font.family: "Inter, Segoe UI, sans-serif" }
                        FilePickField {
                            Layout.fillWidth: true
                            path: settings.ffmpegPathOverride
                            browseTitle: i18n.t("set.ffmpegPath")
                            nameFilters: [
                                "ffmpeg executable (ffmpeg ffmpeg.exe)",
                                "Executables (*.exe)",
                                "All files (*)"
                            ]
                            onFilePicked: function(p) { settings.ffmpegPathOverride = p }
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

    // Reusable login surface for the Sign-in card above.  It's hosted
    // on the page (not inside the AcrylicCard) so its lifecycle is
    // bound to the SettingsPage instance and the WebEngineProfile we
    // assign survives a card re-layout.
    LoginWindow {
        id: loginWindow
    }
}
