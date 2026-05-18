#pragma once

#include <QObject>
#include <QString>
#include <QSettings>

// Persistent user settings, surfaced to QML as the `settings` context
// property.  Every property is automatically persisted via QSettings
// (organization "Volchay" / application "Volchay-Downloader" — see
// main.cpp) the moment it is changed from QML.
//
// The defaults try to do the right thing on first launch: download to
// the user's Videos folder, prefer mp4 video at the source's best
// quality, audio extraction in mp3 320k, no proxy, no cookies.
class Settings : public QObject
{
    Q_OBJECT

    // Theme / UI
    Q_PROPERTY(QString themeMode      READ themeMode      WRITE setThemeMode      NOTIFY themeModeChanged)
    Q_PROPERTY(bool    compact        READ compact        WRITE setCompact        NOTIFY compactChanged)
    Q_PROPERTY(QString language       READ language       WRITE setLanguage       NOTIFY languageChanged)

    // Output / naming
    Q_PROPERTY(QString outputDir         READ outputDir         WRITE setOutputDir         NOTIFY outputDirChanged)
    Q_PROPERTY(QString filenameTemplate  READ filenameTemplate  WRITE setFilenameTemplate  NOTIFY filenameTemplateChanged)
    Q_PROPERTY(bool    restrictFilenames READ restrictFilenames WRITE setRestrictFilenames NOTIFY restrictFilenamesChanged)
    Q_PROPERTY(bool    organizeByUploader READ organizeByUploader WRITE setOrganizeByUploader NOTIFY organizeByUploaderChanged)

    // Format / quality
    Q_PROPERTY(QString defaultFormat   READ defaultFormat   WRITE setDefaultFormat   NOTIFY defaultFormatChanged)
    Q_PROPERTY(QString containerVideo  READ containerVideo  WRITE setContainerVideo  NOTIFY containerVideoChanged)
    Q_PROPERTY(QString audioFormat     READ audioFormat     WRITE setAudioFormat     NOTIFY audioFormatChanged)
    Q_PROPERTY(int     audioQuality    READ audioQuality    WRITE setAudioQuality    NOTIFY audioQualityChanged)
    Q_PROPERTY(bool    preferFreeFormats READ preferFreeFormats WRITE setPreferFreeFormats NOTIFY preferFreeFormatsChanged)

    // Extras (subtitles / thumbnails / metadata / chapters)
    Q_PROPERTY(bool writeSubs       READ writeSubs       WRITE setWriteSubs       NOTIFY writeSubsChanged)
    Q_PROPERTY(bool writeAutoSubs   READ writeAutoSubs   WRITE setWriteAutoSubs   NOTIFY writeAutoSubsChanged)
    Q_PROPERTY(bool embedSubs       READ embedSubs       WRITE setEmbedSubs       NOTIFY embedSubsChanged)
    Q_PROPERTY(QString subLangs     READ subLangs        WRITE setSubLangs        NOTIFY subLangsChanged)
    Q_PROPERTY(bool writeThumbnail  READ writeThumbnail  WRITE setWriteThumbnail  NOTIFY writeThumbnailChanged)
    Q_PROPERTY(bool embedThumbnail  READ embedThumbnail  WRITE setEmbedThumbnail  NOTIFY embedThumbnailChanged)
    Q_PROPERTY(bool embedMetadata   READ embedMetadata   WRITE setEmbedMetadata   NOTIFY embedMetadataChanged)
    Q_PROPERTY(bool embedChapters   READ embedChapters   WRITE setEmbedChapters   NOTIFY embedChaptersChanged)
    Q_PROPERTY(bool sponsorblock    READ sponsorblock    WRITE setSponsorblock    NOTIFY sponsorblockChanged)

    // Network / runtime
    Q_PROPERTY(int     maxConcurrent       READ maxConcurrent       WRITE setMaxConcurrent       NOTIFY maxConcurrentChanged)
    Q_PROPERTY(int     speedLimitKBps      READ speedLimitKBps      WRITE setSpeedLimitKBps      NOTIFY speedLimitKBpsChanged)
    Q_PROPERTY(int     retries             READ retries             WRITE setRetries             NOTIFY retriesChanged)
    Q_PROPERTY(QString proxyUrl            READ proxyUrl            WRITE setProxyUrl            NOTIFY proxyUrlChanged)
    Q_PROPERTY(QString cookiesFromBrowser  READ cookiesFromBrowser  WRITE setCookiesFromBrowser  NOTIFY cookiesFromBrowserChanged)
    Q_PROPERTY(QString cookiesFile         READ cookiesFile         WRITE setCookiesFile         NOTIFY cookiesFileChanged)
    Q_PROPERTY(QString userAgent           READ userAgent           WRITE setUserAgent           NOTIFY userAgentChanged)

    // Advanced
    Q_PROPERTY(QString ytDlpPathOverride READ ytDlpPathOverride WRITE setYtDlpPathOverride NOTIFY ytDlpPathOverrideChanged)
    Q_PROPERTY(QString ffmpegPathOverride READ ffmpegPathOverride WRITE setFfmpegPathOverride NOTIFY ffmpegPathOverrideChanged)
    Q_PROPERTY(QString extraArgs           READ extraArgs           WRITE setExtraArgs           NOTIFY extraArgsChanged)

public:
    explicit Settings(QObject *parent = nullptr);

    // -- Theme / UI --
    QString themeMode() const { return m_themeMode; }
    void    setThemeMode(const QString &v);
    bool    compact() const { return m_compact; }
    void    setCompact(bool v);
    QString language() const { return m_language; }
    void    setLanguage(const QString &v);

    // -- Output / naming --
    QString outputDir() const { return m_outputDir; }
    void    setOutputDir(const QString &v);
    QString filenameTemplate() const { return m_filenameTemplate; }
    void    setFilenameTemplate(const QString &v);
    bool    restrictFilenames() const { return m_restrictFilenames; }
    void    setRestrictFilenames(bool v);
    bool    organizeByUploader() const { return m_organizeByUploader; }
    void    setOrganizeByUploader(bool v);

    // -- Format / quality --
    QString defaultFormat() const { return m_defaultFormat; }
    void    setDefaultFormat(const QString &v);
    QString containerVideo() const { return m_containerVideo; }
    void    setContainerVideo(const QString &v);
    QString audioFormat() const { return m_audioFormat; }
    void    setAudioFormat(const QString &v);
    int     audioQuality() const { return m_audioQuality; }
    void    setAudioQuality(int v);
    bool    preferFreeFormats() const { return m_preferFreeFormats; }
    void    setPreferFreeFormats(bool v);

    // -- Extras --
    bool writeSubs() const { return m_writeSubs; }
    void setWriteSubs(bool v);
    bool writeAutoSubs() const { return m_writeAutoSubs; }
    void setWriteAutoSubs(bool v);
    bool embedSubs() const { return m_embedSubs; }
    void setEmbedSubs(bool v);
    QString subLangs() const { return m_subLangs; }
    void    setSubLangs(const QString &v);
    bool writeThumbnail() const { return m_writeThumbnail; }
    void setWriteThumbnail(bool v);
    bool embedThumbnail() const { return m_embedThumbnail; }
    void setEmbedThumbnail(bool v);
    bool embedMetadata() const { return m_embedMetadata; }
    void setEmbedMetadata(bool v);
    bool embedChapters() const { return m_embedChapters; }
    void setEmbedChapters(bool v);
    bool sponsorblock() const { return m_sponsorblock; }
    void setSponsorblock(bool v);

    // -- Network / runtime --
    int     maxConcurrent() const { return m_maxConcurrent; }
    void    setMaxConcurrent(int v);
    int     speedLimitKBps() const { return m_speedLimitKBps; }
    void    setSpeedLimitKBps(int v);
    int     retries() const { return m_retries; }
    void    setRetries(int v);
    QString proxyUrl() const { return m_proxyUrl; }
    void    setProxyUrl(const QString &v);
    QString cookiesFromBrowser() const { return m_cookiesFromBrowser; }
    void    setCookiesFromBrowser(const QString &v);
    QString cookiesFile() const { return m_cookiesFile; }
    void    setCookiesFile(const QString &v);
    QString userAgent() const { return m_userAgent; }
    void    setUserAgent(const QString &v);

    // -- Advanced --
    QString ytDlpPathOverride() const { return m_ytDlpPathOverride; }
    void    setYtDlpPathOverride(const QString &v);
    QString ffmpegPathOverride() const { return m_ffmpegPathOverride; }
    void    setFfmpegPathOverride(const QString &v);
    QString extraArgs() const { return m_extraArgs; }
    void    setExtraArgs(const QString &v);

    // Reset every key to the built-in defaults.
    Q_INVOKABLE void resetToDefaults();

    // Helpers exposed to QML / Settings page.
    Q_INVOKABLE QStringList formatChoices() const;
    Q_INVOKABLE QStringList containerVideoChoices() const;
    Q_INVOKABLE QStringList audioFormatChoices() const;
    Q_INVOKABLE QStringList cookiesBrowserChoices() const;
    Q_INVOKABLE QStringList themeChoices() const;
    Q_INVOKABLE QStringList languageChoices() const;

signals:
    void themeModeChanged();
    void compactChanged();
    void languageChanged();
    void outputDirChanged();
    void filenameTemplateChanged();
    void restrictFilenamesChanged();
    void organizeByUploaderChanged();
    void defaultFormatChanged();
    void containerVideoChanged();
    void audioFormatChanged();
    void audioQualityChanged();
    void preferFreeFormatsChanged();
    void writeSubsChanged();
    void writeAutoSubsChanged();
    void embedSubsChanged();
    void subLangsChanged();
    void writeThumbnailChanged();
    void embedThumbnailChanged();
    void embedMetadataChanged();
    void embedChaptersChanged();
    void sponsorblockChanged();
    void maxConcurrentChanged();
    void speedLimitKBpsChanged();
    void retriesChanged();
    void proxyUrlChanged();
    void cookiesFromBrowserChanged();
    void cookiesFileChanged();
    void userAgentChanged();
    void ytDlpPathOverrideChanged();
    void ffmpegPathOverrideChanged();
    void extraArgsChanged();

private:
    void load();
    template <typename T> void save(const QString &k, const T &v);

    // Theme / UI
    QString m_themeMode      = QStringLiteral("dark");
    bool    m_compact        = false;
    QString m_language       = QStringLiteral("ru");

    // Output / naming
    QString m_outputDir;
    QString m_filenameTemplate = QStringLiteral("%(title)s [%(id)s].%(ext)s");
    bool    m_restrictFilenames = false;
    bool    m_organizeByUploader = false;

    // Format / quality
    QString m_defaultFormat   = QStringLiteral("best");        // best/2160/1440/1080/720/480/360/audio
    QString m_containerVideo  = QStringLiteral("mp4");         // mp4/mkv/webm
    QString m_audioFormat     = QStringLiteral("mp3");         // mp3/m4a/opus/wav/flac
    int     m_audioQuality    = 0;                              // 0 = best (yt-dlp)
    bool    m_preferFreeFormats = false;

    // Extras
    bool    m_writeSubs        = false;
    bool    m_writeAutoSubs    = false;
    bool    m_embedSubs        = false;
    QString m_subLangs         = QStringLiteral("en,ru");
    bool    m_writeThumbnail   = false;
    bool    m_embedThumbnail   = false;
    bool    m_embedMetadata    = true;
    bool    m_embedChapters    = true;
    bool    m_sponsorblock     = false;

    // Network / runtime
    int     m_maxConcurrent      = 2;
    int     m_speedLimitKBps     = 0;
    int     m_retries            = 10;
    QString m_proxyUrl;
    QString m_cookiesFromBrowser = QStringLiteral("none");      // none/chrome/firefox/edge/brave/opera/vivaldi/safari
    QString m_cookiesFile;
    QString m_userAgent;

    // Advanced
    QString m_ytDlpPathOverride;
    QString m_ffmpegPathOverride;
    QString m_extraArgs;
};
