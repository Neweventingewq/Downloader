#pragma once

#include <QAbstractListModel>
#include <QVector>

#include "DownloadJob.h"

// A QAbstractListModel exposing the current download queue (active +
// finished + failed) to QML. The model is the *single source of truth*
// for job state — DownloadManager mutates rows via the model's helpers
// (addJob / updateJob / removeJob), and the QML side just reads.
class DownloadQueueModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int activeCount READ activeCount NOTIFY countsChanged)
    Q_PROPERTY(int queuedCount READ queuedCount NOTIFY countsChanged)
    Q_PROPERTY(int finishedCount READ finishedCount NOTIFY countsChanged)
    Q_PROPERTY(int failedCount   READ failedCount   NOTIFY countsChanged)
    Q_PROPERTY(int totalCount    READ totalCount    NOTIFY countsChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        UrlRole,
        TitleRole,
        UploaderRole,
        ThumbnailUrlRole,
        DurationMsRole,
        TotalBytesRole,
        DownloadedBytesRole,
        SpeedBpsRole,
        EtaSecRole,
        ProgressRole,
        StatusRole,
        StatusTextRole,
        OutputFileRole,
        OutputDirRole,
        FormatChoiceRole,
        ErrorTextRole,
        ErrorKeyRole,
        StartedAtRole,
        FinishedAtRole,
        LogTailRole,
        IsActiveRole,
        IsTerminalRole,
    };

    explicit DownloadQueueModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int activeCount() const;
    int queuedCount() const;
    int finishedCount() const;
    int failedCount() const;
    int totalCount() const { return m_jobs.size(); }

    // Mutators used by DownloadManager.
    void addJob(const DownloadJob &job);
    void updateJob(int id, const DownloadJob &job);
    void mergeJob(int id, std::function<void(DownloadJob&)> mutator);
    bool removeJobById(int id);

    int findRowById(int id) const;
    DownloadJob jobAt(int row) const;
    DownloadJob jobById(int id) const;

    // QML helpers.
    Q_INVOKABLE int  jobIdAt(int row) const;
    Q_INVOKABLE void clearFinished();
    Q_INVOKABLE void clearFailed();
    Q_INVOKABLE void clearAll();

signals:
    void countsChanged();

private:
    void emitChangedFor(int row);

    QVector<DownloadJob> m_jobs;
};
