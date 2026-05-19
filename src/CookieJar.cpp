#include "CookieJar.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>
#include <QUrl>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>

namespace {

// Where the persistent web profile lives on disk.  Stable across
// app restarts: cookies survive reboots, just like a real browser
// profile.  On Windows this resolves to
//   %LOCALAPPDATA%\Volchay\Volchay-Downloader\web-profile\
// and on Linux to
//   ~/.local/share/Volchay/Volchay-Downloader/web-profile/
QString persistentProfileDir()
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (base.isEmpty()) base = QDir::tempPath();
    QDir().mkpath(base);
    return QDir(base).filePath(QStringLiteral("web-profile"));
}

QString netscapeCookiesPath()
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (base.isEmpty()) base = QDir::tempPath();
    QDir().mkpath(base);
    return QDir(base).filePath(QStringLiteral("yt-cookies.txt"));
}

} // namespace

CookieJar::CookieJar(QObject *parent)
    : QObject(parent)
{
    const QString profileDir = persistentProfileDir();
    QDir().mkpath(profileDir);

    // A named, persistent profile.  The first ctor arg is the storage
    // name; passing it makes WebEngine write its cookie store, cache
    // and indexedDB to a stable folder we control — vs the anonymous
    // ctor which creates an in-memory profile that vanishes on quit.
    m_profile = new QWebEngineProfile(QStringLiteral("Volchay-WebProfile"), this);
    m_profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
    m_profile->setPersistentStoragePath(profileDir);
    m_profile->setCachePath(QDir(profileDir).filePath(QStringLiteral("cache")));

    // A modern desktop UA so Google's anti-bot heuristics see us as a
    // real browser; matches the Chrome major we typically ship next to.
    m_profile->setHttpUserAgent(QStringLiteral(
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
        "(KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36"));

    QWebEngineCookieStore *store = m_profile->cookieStore();
    connect(store, &QWebEngineCookieStore::cookieAdded,
            this,  &CookieJar::onCookieAdded);
    connect(store, &QWebEngineCookieStore::cookieRemoved,
            this,  &CookieJar::onCookieRemoved);

    // loadAllCookies is the standard "rehydrate me from disk" trigger
    // — without it the cookieAdded events won't fire for already-saved
    // cookies until the user navigates somewhere.
    store->loadAllCookies();
}

CookieJar::~CookieJar() = default;

bool CookieJar::isGoogleDomain(const QString &domain)
{
    const QString d = domain.toLower();
    // Match both `.google.com` (subdomain-bound, leading dot) and
    // `google.com` (exact-host) variants.
    return d.endsWith(QStringLiteral("google.com"))
        || d.endsWith(QStringLiteral("youtube.com"))
        || d.endsWith(QStringLiteral("googlevideo.com"));
}

bool CookieJar::isAuthCookieName(const QString &name)
{
    // The cookies Google/YouTube set on successful sign-in.  Any one
    // of these means "the user is logged in"; we don't try to
    // disambiguate between accounts.
    //
    // Important: LOGIN_INFO is set by *YouTube* (on .youtube.com)
    // and is what yt-dlp's own logged-in heuristic looks for.  Earlier
    // builds missed it and showed "Not signed in" for users who'd
    // clearly logged in successfully.
    return name == QLatin1String("__Secure-3PSID")
        || name == QLatin1String("__Secure-1PSID")
        || name == QLatin1String("__Secure-3PSIDTS")
        || name == QLatin1String("__Secure-1PSIDTS")
        || name == QLatin1String("__Secure-3PAPISID")
        || name == QLatin1String("__Secure-1PAPISID")
        || name == QLatin1String("SID")
        || name == QLatin1String("HSID")
        || name == QLatin1String("SSID")
        || name == QLatin1String("APISID")
        || name == QLatin1String("SAPISID")
        || name == QLatin1String("LOGIN_INFO");
}

void CookieJar::onCookieAdded(const QNetworkCookie &c)
{
    // Append-or-replace the cookie by (name, domain, path) — the same
    // composite key the cookie store itself uses internally.
    for (int i = 0; i < m_cookies.size(); ++i) {
        const QNetworkCookie &x = m_cookies[i];
        if (x.name() == c.name() && x.domain() == c.domain() && x.path() == c.path()) {
            m_cookies[i] = c;
            recomputeState();
            return;
        }
    }
    m_cookies.append(c);
    recomputeState();
}

void CookieJar::onCookieRemoved(const QNetworkCookie &c)
{
    for (int i = 0; i < m_cookies.size(); ++i) {
        const QNetworkCookie &x = m_cookies[i];
        if (x.name() == c.name() && x.domain() == c.domain() && x.path() == c.path()) {
            m_cookies.removeAt(i);
            recomputeState();
            return;
        }
    }
}

void CookieJar::recomputeState()
{
    bool logged = false;
    int  count  = 0;
    for (const QNetworkCookie &c : std::as_const(m_cookies)) {
        if (!isGoogleDomain(c.domain())) continue;
        ++count;
        if (!logged && isAuthCookieName(QString::fromUtf8(c.name()))) {
            logged = true;
        }
    }
    if (logged != m_youtubeLoggedIn) {
        m_youtubeLoggedIn = logged;
        emit youtubeLoggedInChanged();
    }
    if (count != m_trackedCookieCount) {
        m_trackedCookieCount = count;
        emit trackedCookieCountChanged();
    }
}

void CookieJar::refresh()
{
    // Wipe our shadow copy and ask the cookie store to re-emit
    // cookieAdded for every cookie it currently holds.  This is the
    // canonical "resync me with the on-disk store" sequence — used
    // when the user closes LoginWindow or revisits Settings, in case
    // one of the just-set auth cookies came in after the previous
    // signal pump round.
    if (!m_profile) return;
    m_cookies.clear();
    if (m_youtubeLoggedIn) {
        m_youtubeLoggedIn = false;
        emit youtubeLoggedInChanged();
    }
    if (m_trackedCookieCount != 0) {
        m_trackedCookieCount = 0;
        emit trackedCookieCountChanged();
    }
    m_profile->cookieStore()->loadAllCookies();
}

QString CookieJar::exportYouTubeCookiesNetscape()
{
    if (m_cookies.isEmpty()) {
        return QString();
    }

    const QString path = netscapeCookiesPath();
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return QString();
    }
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);

    // Standard Netscape header — yt-dlp checks for this signature when
    // it sniffs the cookie file format.
    out << "# Netscape HTTP Cookie File\n";
    out << "# Generated by Volchay-Downloader; do not edit.\n\n";

    int written = 0;
    for (const QNetworkCookie &c : std::as_const(m_cookies)) {
        if (!isGoogleDomain(c.domain())) continue;

        QString domain = c.domain();
        // A leading dot in the domain field means "match any subdomain"
        // in the Netscape spec — yt-dlp / Python's MozillaCookieJar
        // requires this when the includeSubdomains column is TRUE.
        const bool subdom = domain.startsWith(QLatin1Char('.'));
        if (!subdom && !domain.startsWith(QLatin1Char('.'))) {
            // Cookies that came in without a leading dot are
            // host-only.  Leave them as-is — they'll match only the
            // exact host.
        }

        // Session cookies (no expiration date) have to be exported
        // with a future timestamp; yt-dlp's cookiejar otherwise
        // discards them on load.
        qint64 expiryUnix = 0;
        if (c.expirationDate().isValid()) {
            expiryUnix = c.expirationDate().toSecsSinceEpoch();
        } else {
            expiryUnix = QDateTime::currentSecsSinceEpoch() + 60 * 60 * 24 * 30;
        }

        // Skip cookies whose value contains a TAB — Netscape format
        // is tab-delimited, embedded tabs would corrupt the file.
        const QString value = QString::fromUtf8(c.value());
        if (value.contains(QLatin1Char('\t'))) continue;

        out << domain << '\t'
            << (subdom ? QStringLiteral("TRUE") : QStringLiteral("FALSE")) << '\t'
            << c.path() << '\t'
            << (c.isSecure() ? QStringLiteral("TRUE") : QStringLiteral("FALSE")) << '\t'
            << expiryUnix << '\t'
            << QString::fromUtf8(c.name()) << '\t'
            << value << '\n';
        ++written;
    }
    f.close();

    if (written == 0) {
        // No cookies actually written — clean up the stub file so we
        // don't lie to yt-dlp about having credentials.
        QFile::remove(path);
        return QString();
    }
    return path;
}

void CookieJar::signOutYouTube()
{
    QWebEngineCookieStore *store = m_profile->cookieStore();
    // Make a copy because deleteCookie() mutates the live list via
    // cookieRemoved.
    const QList<QNetworkCookie> snapshot = m_cookies;
    for (const QNetworkCookie &c : snapshot) {
        if (isGoogleDomain(c.domain())) {
            store->deleteCookie(c);
        }
    }
    // Also clear the on-disk Netscape file so a stale copy doesn't
    // get reused by a subsequent yt-dlp launch.
    QFile::remove(netscapeCookiesPath());
}
