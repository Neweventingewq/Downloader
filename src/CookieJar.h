#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QNetworkCookie>
#include <QPointer>
#include <QWebEngineProfile>

// CookieJar owns a single QWebEngineProfile with persistent storage,
// shared between (a) the embedded LoginWindow that lets the user log in
// to YouTube/Google in-app, and (b) the DownloadManager which writes a
// fresh Netscape `cookies.txt` from this profile right before yt-dlp
// runs.  The persistent profile lives next to the app's other data
// (`AppLocalDataLocation/web-profile/`), completely isolated from the
// user's main Chrome/Firefox — which means it never trips Chrome 127+
// App-Bound Encryption (DPAPI) on disk.
//
// Login detection is intentionally cheap: we treat the presence of a
// `__Secure-3PSI*` / `SID` Google auth cookie under `.youtube.com` or
// `.google.com` as "signed in".  Extracting the email out of the
// session would require a JS hop into accounts.google.com that breaks
// every time Google reshuffles their DOM — not worth it.
class CookieJar : public QObject
{
    Q_OBJECT

    // True when at least one Google/YouTube session auth cookie is
    // present.  See isAuthCookieName() in the .cpp for the exact list.
    Q_PROPERTY(bool youtubeLoggedIn READ youtubeLoggedIn NOTIFY youtubeLoggedInChanged)

    // Number of cookies currently tracked for YouTube/Google/googlevideo
    // domains.  Useful purely as a diagnostic — surfaced in Settings
    // so the user can see whether anything was captured from the
    // sign-in flow even when the login-state heuristic decides "not
    // signed in".
    Q_PROPERTY(int trackedCookieCount READ trackedCookieCount NOTIFY trackedCookieCountChanged)

    // The WebEngine profile bound to this jar.  Exposed so LoginWindow
    // QML can wire `webEngineView.profile = cookieJar.profile`.
    // Type has to be the concrete QWebEngineProfile* (not QObject*) so
    // the QML WebEngineView accepts the binding without coercion.
    Q_PROPERTY(QWebEngineProfile *profile READ profile CONSTANT)

public:
    explicit CookieJar(QObject *parent = nullptr);
    ~CookieJar() override;

    bool youtubeLoggedIn() const { return m_youtubeLoggedIn; }
    int  trackedCookieCount() const { return m_trackedCookieCount; }
    QWebEngineProfile *profile() const { return m_profile; }

    // Writes the YouTube/Google cookies of this profile into a
    // Netscape-format cookies.txt file at a stable per-app path
    // (AppLocalDataLocation/yt-cookies.txt).  Returns the absolute
    // path on success, or an empty string if no cookies were
    // exportable.  Called by DownloadManager right before each
    // yt-dlp launch so the file is always fresh.
    Q_INVOKABLE QString exportYouTubeCookiesNetscape();

    // Deletes every cookie under .google.com / .youtube.com from the
    // profile.  Used by the "Sign out of YouTube" button in Settings.
    Q_INVOKABLE void signOutYouTube();

    // Force a re-read of every cookie in the WebEngine cookie store.
    // Re-emits cookieAdded for already-stored cookies; we use it on
    // LoginWindow.close and on SettingsPage.show in case the event
    // for the just-stored auth cookie didn't fire synchronously
    // before the user clicked away.  Cheap (microseconds), always
    // safe to call.
    Q_INVOKABLE void refresh();

signals:
    void youtubeLoggedInChanged();
    void trackedCookieCountChanged();

private slots:
    void onCookieAdded(const QNetworkCookie &c);
    void onCookieRemoved(const QNetworkCookie &c);

private:
    static bool isGoogleDomain(const QString &domain);
    static bool isAuthCookieName(const QString &name);
    void recomputeState();

    QWebEngineProfile *m_profile = nullptr;
    QList<QNetworkCookie> m_cookies;
    bool m_youtubeLoggedIn = false;
    int  m_trackedCookieCount = 0;
};
