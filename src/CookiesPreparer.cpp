#include "CookiesPreparer.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QString>
#include <QThread>

// ---------------------------------------------------------------------------
//  Construction / destruction
// ---------------------------------------------------------------------------

CookiesPreparer::CookiesPreparer() = default;

CookiesPreparer::~CookiesPreparer()
{
    cleanupAll();
}

// ---------------------------------------------------------------------------
//  Public helpers
// ---------------------------------------------------------------------------

QString CookiesPreparer::normalizeBrowserName(const QString &browser)
{
    const QString s = browser.trimmed().toLower();
    static const QStringList kKnown = {
        QStringLiteral("chrome"),  QStringLiteral("chromium"),
        QStringLiteral("edge"),    QStringLiteral("brave"),
        QStringLiteral("opera"),   QStringLiteral("vivaldi"),
        QStringLiteral("firefox"), QStringLiteral("safari"),
    };
    return kKnown.contains(s) ? s : QString();
}

bool CookiesPreparer::isChromium(const QString &browser)
{
    const QString s = normalizeBrowserName(browser);
    return s == QStringLiteral("chrome")
        || s == QStringLiteral("chromium")
        || s == QStringLiteral("edge")
        || s == QStringLiteral("brave")
        || s == QStringLiteral("opera")
        || s == QStringLiteral("vivaldi");
}

// ---------------------------------------------------------------------------
//  Default user-data dir resolution
// ---------------------------------------------------------------------------

QString CookiesPreparer::defaultChromiumUserDataDir(const QString &browser)
{
    const QString b = normalizeBrowserName(browser);
    if (b.isEmpty()) return {};

    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#if defined(Q_OS_WIN)
    const QString localAppData = env.value(QStringLiteral("LOCALAPPDATA"));
    const QString appData      = env.value(QStringLiteral("APPDATA"));
    if (b == QStringLiteral("chrome"))   return QDir(localAppData).filePath("Google/Chrome/User Data");
    if (b == QStringLiteral("chromium")) return QDir(localAppData).filePath("Chromium/User Data");
    if (b == QStringLiteral("edge"))     return QDir(localAppData).filePath("Microsoft/Edge/User Data");
    if (b == QStringLiteral("brave"))    return QDir(localAppData).filePath("BraveSoftware/Brave-Browser/User Data");
    if (b == QStringLiteral("vivaldi"))  return QDir(localAppData).filePath("Vivaldi/User Data");
    if (b == QStringLiteral("opera"))    return QDir(appData).filePath("Opera Software/Opera Stable");
#elif defined(Q_OS_MAC)
    const QString home = QDir::homePath();
    if (b == QStringLiteral("chrome"))   return QDir(home).filePath("Library/Application Support/Google/Chrome");
    if (b == QStringLiteral("chromium")) return QDir(home).filePath("Library/Application Support/Chromium");
    if (b == QStringLiteral("edge"))     return QDir(home).filePath("Library/Application Support/Microsoft Edge");
    if (b == QStringLiteral("brave"))    return QDir(home).filePath("Library/Application Support/BraveSoftware/Brave-Browser");
    if (b == QStringLiteral("vivaldi"))  return QDir(home).filePath("Library/Application Support/Vivaldi");
    if (b == QStringLiteral("opera"))    return QDir(home).filePath("Library/Application Support/com.operasoftware.Opera");
#else
    // Linux / *BSD: $XDG_CONFIG_HOME ?? ~/.config
    QString cfg = env.value(QStringLiteral("XDG_CONFIG_HOME"));
    if (cfg.isEmpty()) cfg = QDir(QDir::homePath()).filePath(QStringLiteral(".config"));
    if (b == QStringLiteral("chrome"))   return QDir(cfg).filePath("google-chrome");
    if (b == QStringLiteral("chromium")) return QDir(cfg).filePath("chromium");
    if (b == QStringLiteral("edge"))     return QDir(cfg).filePath("microsoft-edge");
    if (b == QStringLiteral("brave"))    return QDir(cfg).filePath("BraveSoftware/Brave-Browser");
    if (b == QStringLiteral("vivaldi"))  return QDir(cfg).filePath("vivaldi");
    if (b == QStringLiteral("opera"))    return QDir(cfg).filePath("opera");
#endif
    return {};
}

QString CookiesPreparer::defaultFirefoxProfilesDir()
{
#if defined(Q_OS_WIN)
    const QString appData = QProcessEnvironment::systemEnvironment()
                                .value(QStringLiteral("APPDATA"));
    return QDir(appData).filePath("Mozilla/Firefox/Profiles");
#elif defined(Q_OS_MAC)
    return QDir(QDir::homePath()).filePath("Library/Application Support/Firefox/Profiles");
#else
    return QDir(QDir::homePath()).filePath(".mozilla/firefox");
#endif
}

// ---------------------------------------------------------------------------
//  Snapshot machinery
// ---------------------------------------------------------------------------

bool CookiesPreparer::copyWithRetries(const QString &src, const QString &dst, int attempts)
{
    if (!QFile::exists(src)) return false;
    for (int i = 0; i < attempts; ++i) {
        if (QFile::exists(dst)) QFile::remove(dst);
        if (QFile::copy(src, dst)) return true;
        // Brief backoff (100 / 200 / 300 / 400 ms ...) to ride out the
        // Chromium write window that holds the file exclusively.  We're
        // already on a background QProcess pump so a few hundred ms of
        // sleep here doesn't block the UI thread.
        QThread::msleep(100 * (i + 1));
    }
    return false;
}

QString CookiesPreparer::snapshotChromiumProfile(const QString &userDataDir,
                                                 const QString &profileName,
                                                 const QString &dst)
{
    const QDir userDir(userDataDir);
    if (!userDir.exists()) return {};

    QDir(dst).removeRecursively();
    QDir().mkpath(dst);

    // Local State sits at the top of the user-data dir and contains the
    // (DPAPI-wrapped on Windows) AES key yt-dlp needs to decrypt v10+
    // cookie values.  Required for Chromium decryption to succeed.
    const QString localStateSrc = userDir.filePath(QStringLiteral("Local State"));
    const QString localStateDst = QDir(dst).filePath(QStringLiteral("Local State"));
    if (QFile::exists(localStateSrc)) {
        if (!copyWithRetries(localStateSrc, localStateDst)) {
            qWarning().noquote() << "CookiesPreparer: failed to copy"
                                 << localStateSrc << "->" << localStateDst;
            // We continue: yt-dlp can still read v10-cookies on some
            // platforms (Linux DBus keyring, macOS Keychain) without
            // Local State, and unencrypted v9 cookies always work.
        }
    }

    // Copy the one profile sub-dir yt-dlp will read from.
    const QString srcProfile = userDir.filePath(profileName);
    if (!QFileInfo(srcProfile).isDir()) return {};

    const QString dstProfile = QDir(dst).filePath(profileName);
    if (!QDir().mkpath(dstProfile)) return {};

    // Files yt-dlp actually touches.  We list them explicitly instead of
    // mirroring the whole profile (which can be hundreds of MiB and
    // contain unrelated state) — only Cookies + its sidecar journals and
    // their WAL/SHM siblings matter for cookie extraction.
    static const QStringList kProfileFiles = {
        QStringLiteral("Cookies"),
        QStringLiteral("Cookies-journal"),
        QStringLiteral("Cookies-wal"),
        QStringLiteral("Cookies-shm"),
        QStringLiteral("Network/Cookies"),
        QStringLiteral("Network/Cookies-journal"),
        QStringLiteral("Network/Cookies-wal"),
        QStringLiteral("Network/Cookies-shm"),
    };

    bool gotCookies = false;
    for (const QString &rel : kProfileFiles) {
        const QString s = QDir(srcProfile).filePath(rel);
        if (!QFile::exists(s)) continue;
        const QString d = QDir(dstProfile).filePath(rel);
        QDir().mkpath(QFileInfo(d).absolutePath());
        if (copyWithRetries(s, d)) {
            if (rel.endsWith(QStringLiteral("Cookies"))) gotCookies = true;
        }
    }

    if (!gotCookies) {
        qWarning().noquote() << "CookiesPreparer: could not copy any Cookies file from"
                             << srcProfile;
        return {};
    }

    return dstProfile;
}

// ---------------------------------------------------------------------------
//  Public API
// ---------------------------------------------------------------------------

QStringList CookiesPreparer::prepareArgs(int jobId, const QString &browser)
{
    const QString b = normalizeBrowserName(browser);
    if (b.isEmpty()) return {};

    // Non-Chromium browsers don't suffer from the locking problem in the
    // same way: Firefox releases its cookies.sqlite for shared reads and
    // Safari is read by yt-dlp through macOS APIs that don't hit the
    // sqlite file directly.  Pass them straight through.
    if (!isChromium(b)) {
        return { QStringLiteral("--cookies-from-browser"), b };
    }

    const QString userDataDir = defaultChromiumUserDataDir(b);
    if (userDataDir.isEmpty() || !QDir(userDataDir).exists()) {
        qInfo().noquote() << "CookiesPreparer: no default user-data dir for" << b
                          << "— passing through to yt-dlp.";
        return { QStringLiteral("--cookies-from-browser"), b };
    }

    // The "Default" profile is by far the most common; this matches
    // yt-dlp's behaviour when no profile is explicitly given.  Future
    // versions of Settings can let the user pick a different profile.
    const QString profileName = QStringLiteral("Default");

    const QString tmpRoot = QDir::tempPath();
    const QString jobDir  = QDir(tmpRoot).filePath(QStringLiteral("vd-cookies-%1-%2-%3")
        .arg(b)
        .arg(jobId)
        .arg(QDateTime::currentMSecsSinceEpoch()));

    const QString snapshotted = snapshotChromiumProfile(userDataDir, profileName, jobDir);
    if (snapshotted.isEmpty()) {
        QDir(jobDir).removeRecursively();
        qWarning().noquote() << "CookiesPreparer: snapshot failed for" << b
                             << "user-data dir" << userDataDir
                             << "— falling back to plain --cookies-from-browser.";
        return { QStringLiteral("--cookies-from-browser"), b };
    }

    m_jobTempDirs.insert(jobId, jobDir);
    return { QStringLiteral("--cookies-from-browser"),
             QStringLiteral("%1:%2").arg(b, QDir::toNativeSeparators(snapshotted)) };
}

void CookiesPreparer::cleanupForJob(int jobId)
{
    const auto it = m_jobTempDirs.constFind(jobId);
    if (it == m_jobTempDirs.constEnd()) return;
    QDir(it.value()).removeRecursively();
    m_jobTempDirs.remove(jobId);
}

void CookiesPreparer::cleanupAll()
{
    for (const QString &dir : std::as_const(m_jobTempDirs)) {
        QDir(dir).removeRecursively();
    }
    m_jobTempDirs.clear();
}
