#include "Settings.h"

#include <QSettings>
#include <QStandardPaths>
#include <QDir>

namespace {

QString defaultOutputDir()
{
    QString d = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    if (d.isEmpty()) {
        d = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    }
    if (d.isEmpty()) {
        d = QDir::homePath();
    }
    return d;
}

} // namespace

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    if (m_outputDir.isEmpty()) m_outputDir = defaultOutputDir();
    load();
}

void Settings::load()
{
    QSettings s;
    m_themeMode          = s.value("theme/mode",          m_themeMode).toString();
    m_compact            = s.value("ui/compact",          m_compact).toBool();
    m_language           = s.value("ui/language",         m_language).toString();
    m_outputDir          = s.value("output/dir",          m_outputDir).toString();
    m_filenameTemplate   = s.value("output/template",     m_filenameTemplate).toString();
    m_restrictFilenames  = s.value("output/restrict",     m_restrictFilenames).toBool();
    m_organizeByUploader = s.value("output/byUploader",   m_organizeByUploader).toBool();
    m_defaultFormat      = s.value("format/default",      m_defaultFormat).toString();
    m_containerVideo     = s.value("format/container",    m_containerVideo).toString();
    m_audioFormat        = s.value("format/audio",        m_audioFormat).toString();
    m_audioQuality       = s.value("format/audioQuality", m_audioQuality).toInt();
    m_preferFreeFormats  = s.value("format/preferFree",   m_preferFreeFormats).toBool();
    m_writeSubs          = s.value("subs/write",          m_writeSubs).toBool();
    m_writeAutoSubs      = s.value("subs/writeAuto",      m_writeAutoSubs).toBool();
    m_embedSubs          = s.value("subs/embed",          m_embedSubs).toBool();
    m_subLangs           = s.value("subs/langs",          m_subLangs).toString();
    m_writeThumbnail     = s.value("thumb/write",         m_writeThumbnail).toBool();
    m_embedThumbnail     = s.value("thumb/embed",         m_embedThumbnail).toBool();
    m_embedMetadata      = s.value("meta/embed",          m_embedMetadata).toBool();
    m_embedChapters      = s.value("meta/chapters",       m_embedChapters).toBool();
    m_sponsorblock       = s.value("meta/sponsorblock",   m_sponsorblock).toBool();
    m_maxConcurrent      = s.value("net/maxConcurrent",   m_maxConcurrent).toInt();
    m_speedLimitKBps     = s.value("net/speedLimit",      m_speedLimitKBps).toInt();
    m_retries            = s.value("net/retries",         m_retries).toInt();
    m_proxyUrl           = s.value("net/proxy",           m_proxyUrl).toString();
    m_cookiesFromBrowser = s.value("net/cookiesBrowser",  m_cookiesFromBrowser).toString();
    m_cookiesFile        = s.value("net/cookiesFile",     m_cookiesFile).toString();
    m_userAgent          = s.value("net/userAgent",       m_userAgent).toString();
    m_ytDlpPathOverride  = s.value("adv/ytDlpPath",       m_ytDlpPathOverride).toString();
    m_ffmpegPathOverride = s.value("adv/ffmpegPath",      m_ffmpegPathOverride).toString();
    m_extraArgs          = s.value("adv/extraArgs",       m_extraArgs).toString();
}

template <typename T>
void Settings::save(const QString &k, const T &v)
{
    QSettings().setValue(k, QVariant::fromValue(v));
}

// ---------- setters ----------

#define VD_DEFINE_SETTER(Method, Member, Signal, Key) \
    if (m_##Member == v) return; \
    m_##Member = v; \
    save(QStringLiteral(Key), v); \
    emit Signal();

void Settings::setThemeMode(const QString &v)         { VD_DEFINE_SETTER(setThemeMode, themeMode, themeModeChanged, "theme/mode") }
void Settings::setCompact(bool v)                     { VD_DEFINE_SETTER(setCompact, compact, compactChanged, "ui/compact") }
void Settings::setLanguage(const QString &v)          { VD_DEFINE_SETTER(setLanguage, language, languageChanged, "ui/language") }
void Settings::setOutputDir(const QString &v)         { VD_DEFINE_SETTER(setOutputDir, outputDir, outputDirChanged, "output/dir") }
void Settings::setFilenameTemplate(const QString &v)  { VD_DEFINE_SETTER(setFilenameTemplate, filenameTemplate, filenameTemplateChanged, "output/template") }
void Settings::setRestrictFilenames(bool v)           { VD_DEFINE_SETTER(setRestrictFilenames, restrictFilenames, restrictFilenamesChanged, "output/restrict") }
void Settings::setOrganizeByUploader(bool v)          { VD_DEFINE_SETTER(setOrganizeByUploader, organizeByUploader, organizeByUploaderChanged, "output/byUploader") }
void Settings::setDefaultFormat(const QString &v)     { VD_DEFINE_SETTER(setDefaultFormat, defaultFormat, defaultFormatChanged, "format/default") }
void Settings::setContainerVideo(const QString &v)    { VD_DEFINE_SETTER(setContainerVideo, containerVideo, containerVideoChanged, "format/container") }
void Settings::setAudioFormat(const QString &v)       { VD_DEFINE_SETTER(setAudioFormat, audioFormat, audioFormatChanged, "format/audio") }
void Settings::setAudioQuality(int v)                 { VD_DEFINE_SETTER(setAudioQuality, audioQuality, audioQualityChanged, "format/audioQuality") }
void Settings::setPreferFreeFormats(bool v)           { VD_DEFINE_SETTER(setPreferFreeFormats, preferFreeFormats, preferFreeFormatsChanged, "format/preferFree") }
void Settings::setWriteSubs(bool v)                   { VD_DEFINE_SETTER(setWriteSubs, writeSubs, writeSubsChanged, "subs/write") }
void Settings::setWriteAutoSubs(bool v)               { VD_DEFINE_SETTER(setWriteAutoSubs, writeAutoSubs, writeAutoSubsChanged, "subs/writeAuto") }
void Settings::setEmbedSubs(bool v)                   { VD_DEFINE_SETTER(setEmbedSubs, embedSubs, embedSubsChanged, "subs/embed") }
void Settings::setSubLangs(const QString &v)          { VD_DEFINE_SETTER(setSubLangs, subLangs, subLangsChanged, "subs/langs") }
void Settings::setWriteThumbnail(bool v)              { VD_DEFINE_SETTER(setWriteThumbnail, writeThumbnail, writeThumbnailChanged, "thumb/write") }
void Settings::setEmbedThumbnail(bool v)              { VD_DEFINE_SETTER(setEmbedThumbnail, embedThumbnail, embedThumbnailChanged, "thumb/embed") }
void Settings::setEmbedMetadata(bool v)               { VD_DEFINE_SETTER(setEmbedMetadata, embedMetadata, embedMetadataChanged, "meta/embed") }
void Settings::setEmbedChapters(bool v)               { VD_DEFINE_SETTER(setEmbedChapters, embedChapters, embedChaptersChanged, "meta/chapters") }
void Settings::setSponsorblock(bool v)                { VD_DEFINE_SETTER(setSponsorblock, sponsorblock, sponsorblockChanged, "meta/sponsorblock") }
void Settings::setMaxConcurrent(int v)
{
    if (v < 1) v = 1;
    if (v > 8) v = 8;
    VD_DEFINE_SETTER(setMaxConcurrent, maxConcurrent, maxConcurrentChanged, "net/maxConcurrent")
}
void Settings::setSpeedLimitKBps(int v)
{
    if (v < 0) v = 0;
    VD_DEFINE_SETTER(setSpeedLimitKBps, speedLimitKBps, speedLimitKBpsChanged, "net/speedLimit")
}
void Settings::setRetries(int v)
{
    if (v < 0)   v = 0;
    if (v > 100) v = 100;
    VD_DEFINE_SETTER(setRetries, retries, retriesChanged, "net/retries")
}
void Settings::setProxyUrl(const QString &v)          { VD_DEFINE_SETTER(setProxyUrl, proxyUrl, proxyUrlChanged, "net/proxy") }
void Settings::setCookiesFromBrowser(const QString &v){ VD_DEFINE_SETTER(setCookiesFromBrowser, cookiesFromBrowser, cookiesFromBrowserChanged, "net/cookiesBrowser") }
void Settings::setCookiesFile(const QString &v)       { VD_DEFINE_SETTER(setCookiesFile, cookiesFile, cookiesFileChanged, "net/cookiesFile") }
void Settings::setUserAgent(const QString &v)         { VD_DEFINE_SETTER(setUserAgent, userAgent, userAgentChanged, "net/userAgent") }
void Settings::setYtDlpPathOverride(const QString &v) { VD_DEFINE_SETTER(setYtDlpPathOverride, ytDlpPathOverride, ytDlpPathOverrideChanged, "adv/ytDlpPath") }
void Settings::setFfmpegPathOverride(const QString &v){ VD_DEFINE_SETTER(setFfmpegPathOverride, ffmpegPathOverride, ffmpegPathOverrideChanged, "adv/ffmpegPath") }
void Settings::setExtraArgs(const QString &v)         { VD_DEFINE_SETTER(setExtraArgs, extraArgs, extraArgsChanged, "adv/extraArgs") }

#undef VD_DEFINE_SETTER

void Settings::resetToDefaults()
{
    QSettings().clear();
    // Reset in-memory state to the constructor defaults, then notify.
    m_themeMode          = QStringLiteral("dark");
    m_compact            = false;
    m_language           = QStringLiteral("en");
    m_outputDir          = defaultOutputDir();
    m_filenameTemplate   = QStringLiteral("%(title)s [%(id)s].%(ext)s");
    m_restrictFilenames  = false;
    m_organizeByUploader = false;
    m_defaultFormat      = QStringLiteral("best");
    m_containerVideo     = QStringLiteral("mp4");
    m_audioFormat        = QStringLiteral("mp3");
    m_audioQuality       = 0;
    m_preferFreeFormats  = false;
    m_writeSubs          = false;
    m_writeAutoSubs      = false;
    m_embedSubs          = false;
    m_subLangs           = QStringLiteral("en,uk");
    m_writeThumbnail     = false;
    m_embedThumbnail     = false;
    m_embedMetadata      = true;
    m_embedChapters      = true;
    m_sponsorblock       = false;
    m_maxConcurrent      = 2;
    m_speedLimitKBps     = 0;
    m_retries            = 10;
    m_proxyUrl.clear();
    m_cookiesFromBrowser = QStringLiteral("none");
    m_cookiesFile.clear();
    m_userAgent.clear();
    m_ytDlpPathOverride.clear();
    m_ffmpegPathOverride.clear();
    m_extraArgs.clear();
    emit themeModeChanged();
    emit compactChanged();
    emit languageChanged();
    emit outputDirChanged();
    emit filenameTemplateChanged();
    emit restrictFilenamesChanged();
    emit organizeByUploaderChanged();
    emit defaultFormatChanged();
    emit containerVideoChanged();
    emit audioFormatChanged();
    emit audioQualityChanged();
    emit preferFreeFormatsChanged();
    emit writeSubsChanged();
    emit writeAutoSubsChanged();
    emit embedSubsChanged();
    emit subLangsChanged();
    emit writeThumbnailChanged();
    emit embedThumbnailChanged();
    emit embedMetadataChanged();
    emit embedChaptersChanged();
    emit sponsorblockChanged();
    emit maxConcurrentChanged();
    emit speedLimitKBpsChanged();
    emit retriesChanged();
    emit proxyUrlChanged();
    emit cookiesFromBrowserChanged();
    emit cookiesFileChanged();
    emit userAgentChanged();
    emit ytDlpPathOverrideChanged();
    emit ffmpegPathOverrideChanged();
    emit extraArgsChanged();
}

QStringList Settings::formatChoices() const
{
    return {
        "best", "2160", "1440", "1080", "720", "480", "360", "audio"
    };
}

QStringList Settings::containerVideoChoices() const
{
    return { "mp4", "mkv", "webm" };
}

QStringList Settings::audioFormatChoices() const
{
    return { "mp3", "m4a", "opus", "wav", "flac", "aac", "vorbis" };
}

QStringList Settings::cookiesBrowserChoices() const
{
    return { "none", "chrome", "firefox", "edge", "brave", "opera", "vivaldi", "safari", "chromium" };
}

QStringList Settings::themeChoices() const
{
    return { "snow", "dark", "blackout" };
}

QStringList Settings::languageChoices() const
{
    // English first because it is the default and the fallback used
    // when an unknown locale slips through (e.g. "ru" left behind in
    // QSettings by an older build).
    return { "en", "uk" };
}
