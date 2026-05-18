#include "ToolsLocator.h"
#include "Settings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

namespace {

#ifdef Q_OS_WIN
constexpr const char *kExeSuffix = ".exe";
#else
constexpr const char *kExeSuffix = "";
#endif

} // namespace

ToolsLocator::ToolsLocator(QObject *parent)
    : QObject(parent)
{
    refresh();
}

void ToolsLocator::setSettings(Settings *s)
{
    if (m_settings == s) return;
    m_settings = s;
    if (m_settings) {
        connect(m_settings, &Settings::ytDlpPathOverrideChanged,  this, &ToolsLocator::refresh);
        connect(m_settings, &Settings::ffmpegPathOverrideChanged, this, &ToolsLocator::refresh);
    }
    refresh();
}

QString ToolsLocator::findTool(const QString &nameNoExt, const QString &override) const
{
    // 1. Settings override
    if (!override.isEmpty()) {
        QFileInfo fi(override);
        if (fi.exists() && fi.isFile()) {
            return fi.absoluteFilePath();
        }
    }
    // 2. App directory
    const QString candidate = QDir(QCoreApplication::applicationDirPath())
                                  .filePath(nameNoExt + kExeSuffix);
    if (QFileInfo::exists(candidate)) {
        return QFileInfo(candidate).absoluteFilePath();
    }
    // 3. $PATH
    const QString fromPath = QStandardPaths::findExecutable(nameNoExt);
    if (!fromPath.isEmpty()) return fromPath;

    return {};
}

QString ToolsLocator::runForVersion(const QString &exe, const QStringList &args) const
{
    if (exe.isEmpty()) return {};
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(exe, args);
    if (!p.waitForStarted(3000)) return {};
    if (!p.waitForFinished(5000)) {
        p.kill();
        return {};
    }
    QString out = QString::fromUtf8(p.readAll()).trimmed();
    // Keep just the first line — yt-dlp prints its version on a single
    // line; ffmpeg prints "ffmpeg version 6.1.1 ..." on the first line.
    int nl = out.indexOf(QChar('\n'));
    if (nl >= 0) out = out.left(nl).trimmed();
    return out;
}

void ToolsLocator::refresh()
{
    const QString ytOver  = m_settings ? m_settings->ytDlpPathOverride()  : QString();
    const QString ffOver  = m_settings ? m_settings->ffmpegPathOverride() : QString();

    m_ytDlpPath  = findTool(QStringLiteral("yt-dlp"), ytOver);
    m_ffmpegPath = findTool(QStringLiteral("ffmpeg"), ffOver);

    m_ytDlpVersion  = runForVersion(m_ytDlpPath,  { "--version" });
    m_ffmpegVersion = runForVersion(m_ffmpegPath, { "-version"  });

    emit pathsChanged();
}
