#pragma once

#include <QObject>
#include <QString>

class Settings;

// Locates the external command-line tools the downloader depends on:
//
//   - yt-dlp   — the actual video extractor (Python project)
//   - ffmpeg   — used by yt-dlp when merging adaptive streams and when
//                extracting audio.
//
// Search order, per tool:
//   1. The path the user typed into the Settings page (if non-empty
//      and the file exists / is executable).
//   2. The application directory (i.e. next to the .exe).  This is the
//      path that ships inside the Windows release bundle.
//   3. $PATH, via QStandardPaths::findExecutable.
//
// Exposed to QML as the context property `tools`.
class ToolsLocator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ytDlpPath  READ ytDlpPath  NOTIFY pathsChanged)
    Q_PROPERTY(QString ffmpegPath READ ffmpegPath NOTIFY pathsChanged)
    Q_PROPERTY(QString ytDlpVersion  READ ytDlpVersion  NOTIFY pathsChanged)
    Q_PROPERTY(QString ffmpegVersion READ ffmpegVersion NOTIFY pathsChanged)
    Q_PROPERTY(bool    ytDlpAvailable  READ ytDlpAvailable  NOTIFY pathsChanged)
    Q_PROPERTY(bool    ffmpegAvailable READ ffmpegAvailable NOTIFY pathsChanged)

public:
    explicit ToolsLocator(QObject *parent = nullptr);

    // The caller can attach Settings later (after construction) so that
    // user-supplied overrides take effect.
    void setSettings(Settings *s);

    QString ytDlpPath() const     { return m_ytDlpPath; }
    QString ffmpegPath() const    { return m_ffmpegPath; }
    QString ytDlpVersion() const  { return m_ytDlpVersion; }
    QString ffmpegVersion() const { return m_ffmpegVersion; }
    bool    ytDlpAvailable() const  { return !m_ytDlpPath.isEmpty(); }
    bool    ffmpegAvailable() const { return !m_ffmpegPath.isEmpty(); }

    Q_INVOKABLE void refresh();

signals:
    void pathsChanged();

private:
    QString findTool(const QString &nameNoExt, const QString &override) const;
    QString runForVersion(const QString &exe, const QStringList &args) const;

    Settings *m_settings = nullptr;
    QString m_ytDlpPath;
    QString m_ffmpegPath;
    QString m_ytDlpVersion;
    QString m_ffmpegVersion;
};
