#include "DownloadManager.h"
#include "DownloadQueueModel.h"
#include "Settings.h"
#include "ToolsLocator.h"

#include <QClipboard>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QUrl>

namespace {

// Tag that goes in front of our progress lines so we can pick them out
// of the rest of yt-dlp's output without ambiguity.
constexpr const char *kProgressTag = "VDPROG";

// All progress fields are separated by U+001F (Unit Separator) — a
// non-printable control character that never appears in real titles
// or URLs, so we can split fearlessly.
constexpr QChar kSep = QChar(0x1F);

QString formatTemplate(const QString &dir, const QString &nameTemplate, bool byUploader)
{
    QString d = dir;
    if (byUploader) {
        d = QDir(d).filePath(QStringLiteral("%(uploader)s"));
    }
    return QDir(d).filePath(nameTemplate);
}

// Split a line by kSep and trim each piece.
QStringList splitProgress(QStringView line)
{
    QStringList parts;
    int start = 0;
    for (int i = 0; i < line.size(); ++i) {
        if (line.at(i) == kSep) {
            parts << line.mid(start, i - start).toString().trimmed();
            start = i + 1;
        }
    }
    parts << line.mid(start).toString().trimmed();
    return parts;
}

qint64 parseInt64OrNeg(const QString &s)
{
    bool ok = false;
    qint64 v = s.toLongLong(&ok);
    return ok ? v : -1;
}

double parseDoubleOr0(const QString &s)
{
    bool ok = false;
    double v = s.toDouble(&ok);
    return ok ? v : 0.0;
}

} // namespace

DownloadManager::DownloadManager(Settings *settings,
                                 ToolsLocator *tools,
                                 DownloadQueueModel *model,
                                 QObject *parent)
    : QObject(parent),
      m_settings(settings),
      m_tools(tools),
      m_model(model)
{
    if (m_tools) m_tools->setSettings(m_settings);
    if (m_settings) {
        connect(m_settings, &Settings::maxConcurrentChanged, this, &DownloadManager::pump);
    }
    if (m_tools) {
        connect(m_tools, &ToolsLocator::pathsChanged, this, &DownloadManager::canStartChanged);
    }
}

DownloadManager::~DownloadManager()
{
    // Tear down any child processes still running. We send a graceful
    // terminate first, then kill after 1500 ms; yt-dlp cleans up
    // partial files on SIGTERM/Ctrl-C in most cases.
    for (auto &a : m_active) {
        if (a.process) {
            a.process->terminate();
            if (!a.process->waitForFinished(1500)) a.process->kill();
        }
    }
}

bool DownloadManager::canStart() const
{
    return m_tools && m_tools->ytDlpAvailable();
}

bool DownloadManager::looksLikeSupportedUrl(const QString &url) const
{
    const QString u = url.trimmed();
    if (u.isEmpty()) return false;
    const QUrl q(u, QUrl::TolerantMode);
    if (!q.isValid()) return false;
    const QString scheme = q.scheme().toLower();
    if (scheme != QStringLiteral("http") && scheme != QStringLiteral("https"))
        return false;
    return !q.host().isEmpty();
}

int DownloadManager::enqueue(const QString &url)
{
    const QString fmt = m_settings ? m_settings->defaultFormat() : QStringLiteral("best");
    return enqueueInternal(url, fmt);
}

int DownloadManager::enqueueWithFormat(const QString &url, const QString &formatChoice)
{
    return enqueueInternal(url, formatChoice);
}

int DownloadManager::enqueueInternal(const QString &url, const QString &formatChoice)
{
    if (!looksLikeSupportedUrl(url)) {
        emitToast(QStringLiteral("toast.invalidUrl"));
        return -1;
    }
    if (!canStart()) {
        emitToast(QStringLiteral("toast.noTools"));
        return -1;
    }
    if (!m_settings || m_settings->outputDir().isEmpty()) {
        emitToast(QStringLiteral("toast.noOutputDir"));
        return -1;
    }

    DownloadJob j;
    j.id          = m_nextId++;
    j.url         = url.trimmed();
    j.title       = j.url;     // placeholder until metadata is resolved
    j.formatChoice= formatChoice;
    j.outputDir   = m_settings->outputDir();
    j.status      = DownloadJob::Queued;
    j.statusText  = QStringLiteral("status.queued");
    m_model->addJob(j);
    emitToast(QStringLiteral("toast.added"));
    pump();
    return j.id;
}

void DownloadManager::pump()
{
    if (!m_settings || !m_tools || !m_model) return;
    const int limit = m_settings->maxConcurrent();
    while (m_active.size() < limit) {
        // Find the next queued job (oldest first).
        int next = -1;
        for (int r = 0; r < m_model->rowCount(); ++r) {
            const DownloadJob j = m_model->jobAt(r);
            if (j.status == DownloadJob::Queued) { next = j.id; break; }
        }
        if (next < 0) break;
        startJob(next);
    }
    emit activeCountChanged();
}

void DownloadManager::startJob(int jobId)
{
    if (!m_tools || !m_settings || !m_model) return;
    const QString exe = m_tools->ytDlpPath();
    if (exe.isEmpty()) {
        m_model->mergeJob(jobId, [](DownloadJob &j) {
            j.status     = DownloadJob::Failed;
            j.statusText = QStringLiteral("status.failed");
            j.errorText  = QStringLiteral("yt-dlp not found");
        });
        return;
    }

    DownloadJob seed = m_model->jobById(jobId);
    if (seed.id != jobId) return; // gone

    seed.status       = DownloadJob::Resolving;
    seed.statusText   = QStringLiteral("status.resolving");
    seed.startedAt    = QDateTime::currentDateTime();
    seed.downloadedBytes = 0;
    seed.totalBytes      = -1;
    seed.progress        = 0.0;
    seed.etaSec          = -1;
    seed.speedBps        = 0.0;
    seed.errorText.clear();
    seed.logTail.clear();
    m_model->updateJob(jobId, seed);

    const QStringList args = buildYtDlpArgs(seed);

    auto *proc = new QProcess(this);
    proc->setProgram(exe);
    proc->setArguments(args);
    proc->setProcessChannelMode(QProcess::SeparateChannels);

    connect(proc, &QProcess::readyReadStandardOutput, this, [this, jobId]() {
        onProcessStdout(jobId);
    });
    connect(proc, &QProcess::readyReadStandardError, this, [this, jobId]() {
        onProcessStderr(jobId);
    });
    connect(proc, &QProcess::finished, this,
            [this, jobId](int code, QProcess::ExitStatus status) {
        onProcessFinished(jobId, code, int(status));
    });
    connect(proc, &QProcess::errorOccurred, this, [this, jobId](QProcess::ProcessError err) {
        Q_UNUSED(err);
        // QProcess::errorOccurred is also followed by finished() with a
        // crash exit status; let finished() do the bookkeeping. We just
        // record a hint in the log here.
        if (!m_active.contains(jobId)) return;
        m_model->mergeJob(jobId, [](DownloadJob &j) {
            j.logTail.append(QStringLiteral("[QProcess error]\n"));
        });
    });

    Active a;
    a.jobId = jobId;
    a.process = proc;
    m_active.insert(jobId, a);

    proc->start();
    emit activeCountChanged();
}

QStringList DownloadManager::buildYtDlpArgs(const DownloadJob &j)
{
    QStringList args;
    args << QStringLiteral("--newline")
         << QStringLiteral("--no-colors")
         << QStringLiteral("--encoding") << QStringLiteral("utf-8")
         << QStringLiteral("--no-call-home")
         << QStringLiteral("--no-mtime");

    // Output template
    const QString tmpl = formatTemplate(j.outputDir,
                                        m_settings->filenameTemplate(),
                                        m_settings->organizeByUploader());
    args << QStringLiteral("-o") << tmpl;

    if (m_settings->restrictFilenames()) args << QStringLiteral("--restrict-filenames");

    // Format selection
    const bool isAudio = j.formatChoice == QStringLiteral("audio");
    if (isAudio) {
        args << QStringLiteral("-f") << QStringLiteral("bestaudio/best")
             << QStringLiteral("-x")
             << QStringLiteral("--audio-format") << m_settings->audioFormat()
             << QStringLiteral("--audio-quality") << QString::number(m_settings->audioQuality());
    } else {
        args << QStringLiteral("-f") << resolveFormatSpec(j.formatChoice, m_settings->containerVideo())
             << QStringLiteral("--merge-output-format") << m_settings->containerVideo();
    }
    if (m_settings->preferFreeFormats()) args << QStringLiteral("--prefer-free-formats");

    // Extras
    if (m_settings->writeSubs()) {
        args << QStringLiteral("--write-subs");
        args << QStringLiteral("--sub-langs") << m_settings->subLangs();
    }
    if (m_settings->writeAutoSubs())  args << QStringLiteral("--write-auto-subs");
    if (m_settings->embedSubs())      args << QStringLiteral("--embed-subs");
    if (m_settings->writeThumbnail()) args << QStringLiteral("--write-thumbnail");
    if (m_settings->embedThumbnail()) args << QStringLiteral("--embed-thumbnail");
    if (m_settings->embedMetadata())  args << QStringLiteral("--embed-metadata");
    if (m_settings->embedChapters())  args << QStringLiteral("--embed-chapters");
    if (m_settings->sponsorblock())   args << QStringLiteral("--sponsorblock-remove") << QStringLiteral("sponsor");

    // Network
    if (m_settings->speedLimitKBps() > 0) {
        args << QStringLiteral("--limit-rate")
             << (QString::number(m_settings->speedLimitKBps()) + QStringLiteral("K"));
    }
    args << QStringLiteral("--retries") << QString::number(m_settings->retries());
    if (!m_settings->proxyUrl().isEmpty()) {
        args << QStringLiteral("--proxy") << m_settings->proxyUrl();
    }
    if (m_settings->cookiesFromBrowser() != QStringLiteral("none")
        && !m_settings->cookiesFromBrowser().isEmpty()) {
        // We hand off to CookiesPreparer, which either pre-copies the
        // Chromium profile to a temp directory (and points yt-dlp at
        // that copy via `chrome:/abs/path`) or falls back to passing
        // `--cookies-from-browser <browser>` unchanged.  Either way the
        // returned args go straight onto the yt-dlp command line.
        args << m_cookies.prepareArgs(j.id, m_settings->cookiesFromBrowser());
    }
    if (!m_settings->cookiesFile().isEmpty()) {
        args << QStringLiteral("--cookies") << m_settings->cookiesFile();
    }
    if (!m_settings->userAgent().isEmpty()) {
        args << QStringLiteral("--user-agent") << m_settings->userAgent();
    }

    // ffmpeg location
    if (m_tools->ffmpegAvailable()) {
        args << QStringLiteral("--ffmpeg-location") << QFileInfo(m_tools->ffmpegPath()).absolutePath();
    }

    // Progress template — see kProgressTag / kSep above for the format.
    args << QStringLiteral("--progress")
         << QStringLiteral("--progress-template")
         << QStringLiteral("%1%2%(progress.downloaded_bytes)s%2"
                           "%(progress.total_bytes,progress.total_bytes_estimate)s%2"
                           "%(progress.speed)s%2"
                           "%(progress.eta)s%2"
                           "%(progress.status)s%2"
                           "%(info.title)s%2"
                           "%(info.uploader)s%2"
                           "%(info.duration)s%2"
                           "%(info.thumbnail)s")
                .arg(QString::fromLatin1(kProgressTag), kSep);

    // Print the final destination so we know where it ended up.
    args << QStringLiteral("--print") << QStringLiteral("after_move:%1FINAL%2%(filepath)s")
                                            .arg(QString::fromLatin1(kProgressTag), kSep);

    // Extra free-form user arguments. We split on whitespace; users who
    // need spaces inside an arg can quote it via Bash-style: "--foo bar".
    const QString extras = m_settings->extraArgs().trimmed();
    if (!extras.isEmpty()) {
        static const QRegularExpression rx(QStringLiteral(R"(\"([^\"]*)\"|'([^']*)'|(\S+))"));
        auto it = rx.globalMatch(extras);
        while (it.hasNext()) {
            const auto m = it.next();
            for (int g = 1; g <= 3; ++g) {
                if (m.capturedLength(g)) {
                    args << m.captured(g);
                    break;
                }
            }
        }
    }

    args << j.url;
    return args;
}

QString DownloadManager::resolveFormatSpec(const QString &choice, const QString &container)
{
    Q_UNUSED(container);
    if (choice == QStringLiteral("best")) {
        return QStringLiteral("bv*+ba/b");
    }
    bool isNumeric = false;
    const int h = choice.toInt(&isNumeric);
    if (isNumeric && h > 0) {
        return QString("bv*[height<=%1]+ba/b[height<=%1]/b").arg(h);
    }
    return QStringLiteral("bv*+ba/b");
}

// ---------------------------------------------------------------------------
//  Stream handling
// ---------------------------------------------------------------------------

void DownloadManager::onProcessStdout(int jobId)
{
    auto it = m_active.find(jobId);
    if (it == m_active.end() || !it->process) return;
    QString chunk = QString::fromUtf8(it->process->readAllStandardOutput());
    it->carryStdout.append(chunk);
    while (true) {
        const int nl = it->carryStdout.indexOf(QChar('\n'));
        if (nl < 0) break;
        QString line = it->carryStdout.left(nl);
        it->carryStdout.remove(0, nl + 1);
        if (line.endsWith(QChar('\r'))) line.chop(1);
        parseStdoutLine(jobId, line);
    }
}

void DownloadManager::onProcessStderr(int jobId)
{
    auto it = m_active.find(jobId);
    if (it == m_active.end() || !it->process) return;
    QString chunk = QString::fromUtf8(it->process->readAllStandardError());
    it->carryStderr.append(chunk);
    while (true) {
        const int nl = it->carryStderr.indexOf(QChar('\n'));
        if (nl < 0) break;
        QString line = it->carryStderr.left(nl);
        it->carryStderr.remove(0, nl + 1);
        if (line.endsWith(QChar('\r'))) line.chop(1);
        parseStderrLine(jobId, line);
    }
}

void DownloadManager::parseStdoutLine(int jobId, const QString &line)
{
    // Append every line to the per-job log tail (capped).
    m_model->mergeJob(jobId, [&](DownloadJob &j) {
        j.logTail.append(line);
        j.logTail.append(QChar('\n'));
        if (j.logTail.size() > 65536) {
            j.logTail = j.logTail.right(65536);
        }
    });

    if (line.startsWith(QString::fromLatin1(kProgressTag) + kSep)) {
        parseProgressLine(jobId, line);
        return;
    }

    // Final destination line: "VDPROGFINAL<sep>/path/to/file"
    const QString finalTag = QString("%1FINAL%2").arg(QString::fromLatin1(kProgressTag), kSep);
    if (line.startsWith(finalTag)) {
        const QString path = line.mid(finalTag.size()).trimmed();
        m_model->mergeJob(jobId, [&](DownloadJob &j) {
            j.outputFile = path;
        });
        return;
    }
}

void DownloadManager::parseProgressLine(int jobId, const QString &line)
{
    // Strip the tag prefix.
    const QString stripped = line.mid(QString::fromLatin1(kProgressTag).size() + 1);
    QStringList parts = splitProgress(stripped);
    if (parts.size() < 5) return;

    qint64 dlBytes   = parseInt64OrNeg(parts.value(0));
    qint64 totalBytes= parseInt64OrNeg(parts.value(1));
    double speed     = parseDoubleOr0(parts.value(2));
    qint64 eta       = parseInt64OrNeg(parts.value(3));
    QString status   = parts.value(4);
    QString title    = parts.value(5);
    QString uploader = parts.value(6);
    qint64 durSec    = parseInt64OrNeg(parts.value(7));
    QString thumb    = parts.value(8);

    m_model->mergeJob(jobId, [&](DownloadJob &j) {
        if (!title.isEmpty()    && title    != QStringLiteral("NA")) j.title    = title;
        if (!uploader.isEmpty() && uploader != QStringLiteral("NA")) j.uploader = uploader;
        if (durSec > 0) j.durationMs = durSec * 1000LL;
        if (!thumb.isEmpty() && thumb != QStringLiteral("NA")) j.thumbnailUrl = thumb;

        if (totalBytes > 0)  j.totalBytes = totalBytes;
        if (dlBytes >= 0)    j.downloadedBytes = dlBytes;
        if (speed > 0)       j.speedBps = speed;
        j.etaSec = (eta >= 0) ? int(eta) : -1;

        if (j.totalBytes > 0) {
            j.progress = qBound(0.0, double(j.downloadedBytes) / double(j.totalBytes), 1.0);
        }

        if (status == QStringLiteral("downloading")) {
            j.status = DownloadJob::Downloading;
            j.statusText = QStringLiteral("status.downloading");
        } else if (status == QStringLiteral("finished")) {
            j.status = DownloadJob::PostProcessing;
            j.statusText = QStringLiteral("status.postprocessing");
            // progress shows 100% the moment the file is fully fetched
            // (before any merge / extract step).
            j.progress = 1.0;
        }
    });
}

void DownloadManager::parseStderrLine(int jobId, const QString &line)
{
    // yt-dlp prints "ERROR: ..." on stderr for actual failures.
    //
    // It also prints WARNINGs that, while non-fatal in yt-dlp's eyes,
    // are very often the root cause of the eventual "Failed" status the
    // user sees — most notably the cookie-database lock that's so
    // entrenched it has its own bug tracker entry
    // (yt-dlp issue 7271).  We hoist that one up into a clear, localised
    // error key so the QML side can render a useful explanation instead
    // of the raw English yt-dlp warning text.
    m_model->mergeJob(jobId, [&](DownloadJob &j) {
        j.logTail.append(line);
        j.logTail.append(QChar('\n'));
        if (j.logTail.size() > 65536) j.logTail = j.logTail.right(65536);

        // Cookie-database lock — substring match because yt-dlp prefixes
        // the message with timestamps / WARNING markers that vary
        // between versions, and the browser name is interpolated into
        // the text on some builds ("Chrome", "Edge", ...).
        if (line.contains(QStringLiteral("Could not copy"), Qt::CaseInsensitive)
         && line.contains(QStringLiteral("cookie database"),  Qt::CaseInsensitive)) {
            j.errorKey  = QStringLiteral("error.cookiesLocked");
            j.errorText = line.trimmed();
        }

        // Chrome 127+ App-Bound Encryption — yt-dlp can't unwrap the
        // master cookie key because the wrapping DPAPI blob is gated
        // behind `elevation_service.exe`.  Closing the browser does
        // NOT fix this one (it's a software-level limitation, not a
        // file-lock issue), so we surface a different, more
        // actionable explanation pointing the user at the
        // export-cookies-to-file workaround.  See yt-dlp issue 10927.
        if (line.contains(QStringLiteral("Failed to decrypt"), Qt::CaseInsensitive)
         && line.contains(QStringLiteral("DPAPI"),             Qt::CaseInsensitive)) {
            j.errorKey  = QStringLiteral("error.cookiesDpapi");
            j.errorText = line.trimmed();
        }

        // YouTube anti-bot challenge.  yt-dlp's own message is
        //   "Sign in to confirm you're not a bot.  Use
        //    --cookies-from-browser or --cookies for the authentication."
        // and it usually means either no cookies were supplied or the
        // cookies that were supplied got rejected (the most common
        // upstream cause of that is the DPAPI failure above swallowing
        // the cookie jar silently).  We give the user a single,
        // actionable sentence — same answer as the cookie errors, just
        // phrased from YouTube's angle so the row makes sense even
        // when read in isolation.
        if (line.contains(QStringLiteral("Sign in to confirm you"),   Qt::CaseInsensitive)
         && line.contains(QStringLiteral("re not a bot"),             Qt::CaseInsensitive)) {
            j.errorKey  = QStringLiteral("error.youtubeBotCheck");
            j.errorText = line.trimmed();
        }

        if (line.startsWith(QStringLiteral("ERROR:"))) {
            j.errorText = line.mid(QStringLiteral("ERROR:").size()).trimmed();
        }
    });
}

void DownloadManager::onProcessFinished(int jobId, int exitCode, int exitStatus)
{
    auto it = m_active.find(jobId);
    if (it == m_active.end()) return;

    // Flush any tail bytes we haven't yet split on newline.
    if (!it->carryStdout.isEmpty()) {
        parseStdoutLine(jobId, it->carryStdout);
        it->carryStdout.clear();
    }
    if (!it->carryStderr.isEmpty()) {
        parseStderrLine(jobId, it->carryStderr);
        it->carryStderr.clear();
    }

    if (it->process) {
        it->process->deleteLater();
        it->process = nullptr;
    }

    m_model->mergeJob(jobId, [&](DownloadJob &j) {
        j.finishedAt = QDateTime::currentDateTime();
        const bool crashed = exitStatus != int(QProcess::NormalExit);
        if (j.status == DownloadJob::Canceled) {
            j.statusText = QStringLiteral("status.canceled");
        } else if (exitCode == 0 && !crashed) {
            j.status = DownloadJob::Finished;
            j.statusText = QStringLiteral("status.finished");
            j.progress = 1.0;
        } else {
            j.status = DownloadJob::Failed;
            j.statusText = QStringLiteral("status.failed");
            if (j.errorText.isEmpty()) {
                j.errorText = QStringLiteral("yt-dlp exited with code %1").arg(exitCode);
            }
        }
    });

    // Drop the per-job cookie snapshot now that yt-dlp is done with it.
    // Done unconditionally — both the success and failure paths above
    // are terminal for this job and won't re-read those files.
    m_cookies.cleanupForJob(jobId);

    m_active.remove(jobId);
    emit activeCountChanged();
    pump();
}

void DownloadManager::cancelJob(int jobId)
{
    auto it = m_active.find(jobId);
    if (it != m_active.end() && it->process) {
        // Mark canceled first so the finished() handler knows the
        // non-zero exit code is intentional and doesn't tag it Failed.
        m_model->mergeJob(jobId, [](DownloadJob &j) {
            j.status     = DownloadJob::Canceled;
            j.statusText = QStringLiteral("status.canceled");
        });
        it->process->terminate();
        if (!it->process->waitForFinished(2000)) it->process->kill();
    } else {
        // Was only queued — drop status to canceled directly.
        m_model->mergeJob(jobId, [](DownloadJob &j) {
            if (j.status == DownloadJob::Queued) {
                j.status = DownloadJob::Canceled;
                j.statusText = QStringLiteral("status.canceled");
                j.finishedAt = QDateTime::currentDateTime();
            }
        });
    }
    emit activeCountChanged();
}

void DownloadManager::retryJob(int jobId)
{
    if (!m_model) return;
    m_model->mergeJob(jobId, [](DownloadJob &j) {
        if (j.status == DownloadJob::Failed
         || j.status == DownloadJob::Canceled) {
            j.status         = DownloadJob::Queued;
            j.statusText     = QStringLiteral("status.queued");
            j.errorText.clear();
            j.errorKey.clear();
            j.downloadedBytes= 0;
            j.totalBytes     = -1;
            j.progress       = 0.0;
            j.etaSec         = -1;
            j.speedBps       = 0.0;
            j.logTail.clear();
            j.outputFile.clear();
        }
    });
    pump();
}

void DownloadManager::removeJob(int jobId)
{
    // Don't allow removing a job that's currently running — cancel it
    // first so the file system is left consistent.
    if (m_active.contains(jobId)) {
        cancelJob(jobId);
    }
    if (m_model) m_model->removeJobById(jobId);
}

void DownloadManager::revealInFileManager(const QString &path) const
{
    if (path.isEmpty()) return;
    QFileInfo fi(path);
    QUrl url = QUrl::fromLocalFile(fi.exists() ? fi.absoluteFilePath() : fi.absolutePath());
    QDesktopServices::openUrl(url);
}

void DownloadManager::openOutputDir() const
{
    if (!m_settings) return;
    const QString d = m_settings->outputDir();
    if (!d.isEmpty()) QDesktopServices::openUrl(QUrl::fromLocalFile(d));
}

void DownloadManager::copyToClipboard(const QString &text) const
{
    if (auto *cb = QGuiApplication::clipboard()) cb->setText(text);
}

void DownloadManager::setLastError(const QString &e)
{
    if (m_lastError == e) return;
    m_lastError = e;
    emit lastErrorChanged();
}

void DownloadManager::emitToast(const QString &key)
{
    m_lastToastKey = key;
    emit toastEmitted();
}
