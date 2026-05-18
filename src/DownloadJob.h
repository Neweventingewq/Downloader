#pragma once

#include <QString>
#include <QDateTime>
#include <QMetaType>

// A single download job — the unit the queue model tracks. One QProcess
// instance is created per active job by DownloadManager.
//
// `id` is a monotonically increasing integer assigned by DownloadManager
// when the job is enqueued; it is the stable identity used by QML to
// refer to a row through cancel() / clear() / open*() calls.
struct DownloadJob
{
    enum Status {
        Queued,
        Resolving,    // pre-fetching metadata via yt-dlp -j
        Downloading,
        PostProcessing, // ffmpeg merge / extract audio / embed metadata
        Finished,
        Failed,
        Canceled
    };

    int       id              = 0;
    QString   url;
    QString   title;          // populated after metadata pre-fetch
    QString   uploader;
    QString   thumbnailUrl;
    qint64    durationMs      = -1;     // -1 if unknown
    qint64    totalBytes      = -1;
    qint64    downloadedBytes = 0;
    qreal     speedBps        = 0.0;
    int       etaSec          = -1;
    qreal     progress        = 0.0;    // 0.0..1.0
    Status    status          = Queued;
    QString   statusText;
    QString   outputFile;     // final on-disk path after success
    QString   outputDir;
    QString   formatChoice;   // copied from settings.defaultFormat at enqueue
    QString   errorText;
    QDateTime startedAt;
    QDateTime finishedAt;
    QString   logTail;        // last ~64 KB of stderr+stdout for diagnostics
};

Q_DECLARE_METATYPE(DownloadJob)
