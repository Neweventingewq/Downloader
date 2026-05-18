#pragma once

#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>

// CookiesPreparer
// ----------------
// Works around the well-known yt-dlp warning
//   "Could not copy Chrome cookie database. See https://github.com/yt-dlp/yt-dlp/issues/7271 for more info"
// that occurs when yt-dlp's --cookies-from-browser tries to read a Chromium
// profile while the browser is still running and holds an exclusive file
// lock on its `Cookies` SQLite database.
//
// Strategy
// ~~~~~~~~
// For Chromium-based browsers (chrome / chromium / edge / brave / opera /
// vivaldi) we pre-snapshot the few files yt-dlp actually needs into a
// per-job temporary directory and then point yt-dlp at that snapshot via
// `--cookies-from-browser <browser>:<temp_profile>`.
//
// The copy uses Qt's QFile::copy with a short retry/backoff loop because
// the Cookies / Cookies-journal handles are released for short windows in
// between SQLite write transactions; on Windows that is often enough to
// grab a clean copy where Python's single-shot shutil.copy fails.
//
// If the snapshot fails (browser holds the lock the whole time, paths not
// detected, etc.) we fall back to passing `--cookies-from-browser <browser>`
// untouched and let yt-dlp try its own approach — the user still gets the
// original behaviour, never worse.
//
// Firefox does not lock its cookies file in the same exclusive way, so we
// pass it through unchanged.
//
// Temp directories are owned by this class and cleaned up after each job
// finishes (success or failure) via cleanupForJob().
class CookiesPreparer
{
public:
    CookiesPreparer();
    ~CookiesPreparer();

    // Prepare cookies for a given job.  Returns a QStringList that should
    // be appended verbatim to the yt-dlp argument vector — empty when no
    // browser cookies are configured.
    //
    // On success for a Chromium-based browser the returned args look like
    //   { "--cookies-from-browser", "chrome:/tmp/vd-cookies-<jobId>/Default" }
    // and a per-job temp directory is registered for later cleanup.
    QStringList prepareArgs(int jobId, const QString &browser);

    // Remove the temp directory associated with this job (no-op if none).
    void cleanupForJob(int jobId);

    // Remove every temp directory we are still holding on to.  Called from
    // the destructor; safe to invoke explicitly as well.
    void cleanupAll();

    // The default user-data directory for a Chromium-based browser on the
    // current OS, or an empty string if we cannot resolve it.  Public for
    // unit testing and diagnostics.
    static QString defaultChromiumUserDataDir(const QString &browser);

    // The default Firefox profiles root (the directory that contains all
    // profile sub-folders like xxxxxx.default-release).  Public for the
    // same reason.
    static QString defaultFirefoxProfilesDir();

    // Lower-case canonical browser name, or empty when the input does not
    // match any supported browser.
    static QString normalizeBrowserName(const QString &browser);

    // True when the browser name is one of the Chromium-based ones.
    static bool isChromium(const QString &browser);

private:
    // Snapshot the bits of a Chromium user-data dir that yt-dlp needs
    // (one profile + the top-level "Local State" key store) into `dst`.
    // Returns the absolute path of the snapshotted profile directory on
    // success, or an empty string on failure.
    static QString snapshotChromiumProfile(const QString &userDataDir,
                                           const QString &profileName,
                                           const QString &dst);

    // Copy src → dst, recreating dst each attempt, retrying a few times
    // with a small backoff to ride out brief Chromium write windows.
    static bool copyWithRetries(const QString &src, const QString &dst,
                                int attempts = 5);

    QHash<int, QString> m_jobTempDirs;
};
