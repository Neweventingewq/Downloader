#pragma once

#include <QObject>
#include <QHash>
#include <QPointer>
#include <QQueue>

#include "DownloadJob.h"

class Settings;
class ToolsLocator;
class DownloadQueueModel;
class QProcess;

// Drives the actual download operation by spawning yt-dlp as a child
// process per job and parsing its progress output line by line.
//
// The manager keeps the queue sorted in two logical layers: a FIFO of
// `Queued` jobs and a small pool of currently-running QProcess
// instances (capped by Settings.maxConcurrent). When a job finishes,
// the next queued job is started — no polling, all event-driven.
//
// Exposed to QML as the context property `manager`. The single entry
// point is enqueue() / enqueueWithFormat(); progress is read off the
// `queue` model (DownloadQueueModel).
class DownloadManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int activeCount READ activeCount NOTIFY activeCountChanged)
    Q_PROPERTY(bool canStart READ canStart NOTIFY canStartChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString lastToastKey READ lastToastKey NOTIFY toastEmitted)

public:
    DownloadManager(Settings *settings,
                    ToolsLocator *tools,
                    DownloadQueueModel *model,
                    QObject *parent = nullptr);
    ~DownloadManager() override;

    int     activeCount() const { return m_active.size(); }
    bool    canStart() const;
    QString lastError() const { return m_lastError; }
    QString lastToastKey() const { return m_lastToastKey; }

    // Enqueue with the current Settings.defaultFormat.
    Q_INVOKABLE int enqueue(const QString &url);

    // Enqueue with an explicit per-download format override (any of the
    // Settings.formatChoices() ids — "best", "1080", "audio", ...).
    Q_INVOKABLE int enqueueWithFormat(const QString &url, const QString &formatChoice);

    Q_INVOKABLE void cancelJob(int jobId);
    Q_INVOKABLE void retryJob(int jobId);
    Q_INVOKABLE void removeJob(int jobId);

    Q_INVOKABLE void revealInFileManager(const QString &path) const;
    Q_INVOKABLE void openOutputDir() const;
    Q_INVOKABLE void copyToClipboard(const QString &text) const;

    // Cheap URL validity check used by the Home page to enable / disable
    // the «Download» button.
    Q_INVOKABLE bool looksLikeSupportedUrl(const QString &url) const;

signals:
    void activeCountChanged();
    void canStartChanged();
    void lastErrorChanged();
    void toastEmitted(); // QML binds to lastToastKey via this signal

private slots:
    void pump();

private:
    struct Active {
        int jobId = -1;
        QPointer<QProcess> process;
        QString carryStdout;
        QString carryStderr;
        bool sawDownload = false;
    };

    int  enqueueInternal(const QString &url, const QString &formatChoice);
    void startJob(int jobId);
    void onProcessStdout(int jobId);
    void onProcessStderr(int jobId);
    void onProcessFinished(int jobId, int exitCode, int exitStatus);
    void parseProgressLine(int jobId, const QString &line);
    void parseStdoutLine(int jobId, const QString &line);
    void parseStderrLine(int jobId, const QString &line);
    void setLastError(const QString &e);
    void emitToast(const QString &key);

    QStringList buildYtDlpArgs(const DownloadJob &j) const;
    static QString resolveFormatSpec(const QString &choice, const QString &container);

    Settings           *m_settings  = nullptr;
    ToolsLocator       *m_tools     = nullptr;
    DownloadQueueModel *m_model     = nullptr;

    int                 m_nextId    = 1;
    QHash<int, Active>  m_active;
    QString             m_lastError;
    QString             m_lastToastKey;
};
