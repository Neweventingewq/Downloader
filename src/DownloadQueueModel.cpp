#include "DownloadQueueModel.h"

DownloadQueueModel::DownloadQueueModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DownloadQueueModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_jobs.size();
}

QHash<int, QByteArray> DownloadQueueModel::roleNames() const
{
    return {
        { IdRole,              "jobId"           },
        { UrlRole,             "url"             },
        { TitleRole,           "title"           },
        { UploaderRole,        "uploader"        },
        { ThumbnailUrlRole,    "thumbnailUrl"    },
        { DurationMsRole,      "durationMs"      },
        { TotalBytesRole,      "totalBytes"      },
        { DownloadedBytesRole, "downloadedBytes" },
        { SpeedBpsRole,        "speedBps"        },
        { EtaSecRole,          "etaSec"          },
        { ProgressRole,        "progress"        },
        { StatusRole,          "status"          },
        { StatusTextRole,      "statusText"      },
        { OutputFileRole,      "outputFile"      },
        { OutputDirRole,       "outputDir"       },
        { FormatChoiceRole,    "formatChoice"    },
        { ErrorTextRole,       "errorText"       },
        { ErrorKeyRole,        "errorKey"        },
        { StartedAtRole,       "startedAt"       },
        { FinishedAtRole,      "finishedAt"      },
        { LogTailRole,         "logTail"         },
        { IsActiveRole,        "isActive"        },
        { IsTerminalRole,      "isTerminal"      },
    };
}

QVariant DownloadQueueModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_jobs.size())
        return {};
    const DownloadJob &j = m_jobs[index.row()];
    switch (role) {
    case IdRole:              return j.id;
    case UrlRole:             return j.url;
    case TitleRole:           return j.title;
    case UploaderRole:        return j.uploader;
    case ThumbnailUrlRole:    return j.thumbnailUrl;
    case DurationMsRole:      return j.durationMs;
    case TotalBytesRole:      return j.totalBytes;
    case DownloadedBytesRole: return j.downloadedBytes;
    case SpeedBpsRole:        return j.speedBps;
    case EtaSecRole:          return j.etaSec;
    case ProgressRole:        return j.progress;
    case StatusRole:          return int(j.status);
    case StatusTextRole:      return j.statusText;
    case OutputFileRole:      return j.outputFile;
    case OutputDirRole:       return j.outputDir;
    case FormatChoiceRole:    return j.formatChoice;
    case ErrorTextRole:       return j.errorText;
    case ErrorKeyRole:        return j.errorKey;
    case StartedAtRole:       return j.startedAt;
    case FinishedAtRole:      return j.finishedAt;
    case LogTailRole:         return j.logTail;
    case IsActiveRole:
        return j.status == DownloadJob::Resolving
            || j.status == DownloadJob::Downloading
            || j.status == DownloadJob::PostProcessing;
    case IsTerminalRole:
        return j.status == DownloadJob::Finished
            || j.status == DownloadJob::Failed
            || j.status == DownloadJob::Canceled;
    }
    return {};
}

int DownloadQueueModel::activeCount() const
{
    int n = 0;
    for (const auto &j : m_jobs) {
        if (j.status == DownloadJob::Resolving
         || j.status == DownloadJob::Downloading
         || j.status == DownloadJob::PostProcessing) ++n;
    }
    return n;
}

int DownloadQueueModel::queuedCount() const
{
    int n = 0;
    for (const auto &j : m_jobs) {
        if (j.status == DownloadJob::Queued) ++n;
    }
    return n;
}

int DownloadQueueModel::finishedCount() const
{
    int n = 0;
    for (const auto &j : m_jobs) {
        if (j.status == DownloadJob::Finished) ++n;
    }
    return n;
}

int DownloadQueueModel::failedCount() const
{
    int n = 0;
    for (const auto &j : m_jobs) {
        if (j.status == DownloadJob::Failed
         || j.status == DownloadJob::Canceled) ++n;
    }
    return n;
}

void DownloadQueueModel::addJob(const DownloadJob &job)
{
    beginInsertRows({}, m_jobs.size(), m_jobs.size());
    m_jobs.push_back(job);
    endInsertRows();
    emit countsChanged();
}

int DownloadQueueModel::findRowById(int id) const
{
    for (int i = 0; i < m_jobs.size(); ++i) {
        if (m_jobs[i].id == id) return i;
    }
    return -1;
}

DownloadJob DownloadQueueModel::jobAt(int row) const
{
    if (row < 0 || row >= m_jobs.size()) return {};
    return m_jobs[row];
}

DownloadJob DownloadQueueModel::jobById(int id) const
{
    int r = findRowById(id);
    return r >= 0 ? m_jobs[r] : DownloadJob{};
}

void DownloadQueueModel::updateJob(int id, const DownloadJob &job)
{
    int r = findRowById(id);
    if (r < 0) return;
    m_jobs[r] = job;
    emitChangedFor(r);
    emit countsChanged();
}

void DownloadQueueModel::mergeJob(int id, std::function<void(DownloadJob&)> mutator)
{
    int r = findRowById(id);
    if (r < 0) return;
    mutator(m_jobs[r]);
    emitChangedFor(r);
    emit countsChanged();
}

bool DownloadQueueModel::removeJobById(int id)
{
    int r = findRowById(id);
    if (r < 0) return false;
    beginRemoveRows({}, r, r);
    m_jobs.removeAt(r);
    endRemoveRows();
    emit countsChanged();
    return true;
}

int DownloadQueueModel::jobIdAt(int row) const
{
    if (row < 0 || row >= m_jobs.size()) return -1;
    return m_jobs[row].id;
}

void DownloadQueueModel::clearFinished()
{
    for (int i = m_jobs.size() - 1; i >= 0; --i) {
        if (m_jobs[i].status == DownloadJob::Finished) {
            beginRemoveRows({}, i, i);
            m_jobs.removeAt(i);
            endRemoveRows();
        }
    }
    emit countsChanged();
}

void DownloadQueueModel::clearFailed()
{
    for (int i = m_jobs.size() - 1; i >= 0; --i) {
        if (m_jobs[i].status == DownloadJob::Failed
         || m_jobs[i].status == DownloadJob::Canceled) {
            beginRemoveRows({}, i, i);
            m_jobs.removeAt(i);
            endRemoveRows();
        }
    }
    emit countsChanged();
}

void DownloadQueueModel::clearAll()
{
    for (int i = m_jobs.size() - 1; i >= 0; --i) {
        if (m_jobs[i].status != DownloadJob::Resolving
         && m_jobs[i].status != DownloadJob::Downloading
         && m_jobs[i].status != DownloadJob::PostProcessing) {
            beginRemoveRows({}, i, i);
            m_jobs.removeAt(i);
            endRemoveRows();
        }
    }
    emit countsChanged();
}

void DownloadQueueModel::emitChangedFor(int row)
{
    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {
        TitleRole, UploaderRole, ThumbnailUrlRole, DurationMsRole,
        TotalBytesRole, DownloadedBytesRole, SpeedBpsRole, EtaSecRole,
        ProgressRole, StatusRole, StatusTextRole, OutputFileRole,
        OutputDirRole, FormatChoiceRole, ErrorTextRole, StartedAtRole,
        FinishedAtRole, LogTailRole, IsActiveRole, IsTerminalRole
    });
}
