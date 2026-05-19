import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtWebEngine

// Embedded YouTube/Google sign-in surface.  We open this as a
// standalone Window (not as a modal inside the main app window)
// because Google's sign-in flow internally triggers popups —
// `webEngineView.newWindowRequested` — and a Window root lets us
// reparent those without fighting Qt's modal stack.
//
// The QWebEngineProfile we render with is `cookieJar.profile`, which
// is a persistent, named profile sitting on disk under our app data
// directory.  Cookies the user accrues here survive both this window
// closing and the app restarting; they're also where DownloadManager
// will pull cookies from when it builds the yt-dlp command line.
//
// `openLogin()` is the only public API: call it from Settings.
Window {
    id: root
    width: 980
    height: 720
    minimumWidth: 720
    minimumHeight: 520
    title: i18n.t("signin.title")
    visible: false
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowMinMaxButtonsHint

    // Center over the parent window on first show — calling this every
    // openLogin() is intentional: if the user moved the main window
    // after the last sign-in, the next one re-centers correctly.
    function openLogin() {
        const url = "https://accounts.google.com/v3/signin/identifier?continue=https%3A%2F%2Fwww.youtube.com%2F&service=youtube&hl=en"
        web.url = url
        const screen = Qt.application.screens && Qt.application.screens[0]
        if (screen) {
            root.x = screen.virtualX + (screen.width  - root.width)  / 2
            root.y = screen.virtualY + (screen.height - root.height) / 2
        }
        root.show()
        root.raise()
        root.requestActivate()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Amber warning band, repeated here so the user sees it both
        // in Settings (before opening login) and inside the login
        // window itself (right next to the actual login form, where
        // the temptation to type the main account's password lives).
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: bannerText.implicitHeight + 22
            color: "#3a2d05"
            border.color: "#c79a23"
            border.width: 1
            Text {
                id: bannerText
                anchors.fill: parent
                anchors.margins: 11
                text: i18n.t("signin.warning")
                color: "#ffe7a4"
                font.pixelSize: 12
                font.family: "Inter, Segoe UI, sans-serif"
                wrapMode: Text.WordWrap
            }
        }

        // The browser itself.  Profile is shared with CookieJar so
        // every cookie Google sets here is immediately visible to the
        // C++ side via QWebEngineCookieStore::cookieAdded.
        WebEngineView {
            id: web
            Layout.fillWidth: true
            Layout.fillHeight: true
            profile: cookieJar.profile

            onNewWindowRequested: function(request) {
                // Google's "Use another device" / "Try a different way"
                // flows open dialogs.  We route them into the same view
                // — opening a separate child window would lose the
                // visual context of the warning banner.
                request.openIn(web)
            }
        }

        // Footer with a single Close button.  The user has no other
        // task here — once they're signed in, the cookieJar already
        // observed the auth cookies and DownloadManager will pick
        // them up automatically.  No "Save" step.
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: theme.surface
            border.color: theme.border
            border.width: 1
            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                Text {
                    Layout.fillWidth: true
                    text: cookieJar.youtubeLoggedIn
                          ? i18n.t("signin.window.signedin")
                          : i18n.t("signin.window.notyet")
                    color: cookieJar.youtubeLoggedIn ? "#5fb45f" : theme.textSecondary
                    font.pixelSize: 12
                    font.family: "Inter, Segoe UI, sans-serif"
                }
                Button {
                    text: i18n.t("signin.button.close")
                    onClicked: root.close()
                }
            }
        }
    }
}
