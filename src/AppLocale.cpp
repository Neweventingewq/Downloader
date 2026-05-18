#include "AppLocale.h"
#include "Settings.h"

namespace {
void put(QHash<QString, QHash<QString, QString>> &t,
         const QString &key,
         const QString &ru, const QString &en)
{
    QHash<QString, QString> m;
    m.insert(QStringLiteral("ru"), ru);
    m.insert(QStringLiteral("en"), en);
    t.insert(key, m);
}
} // namespace

Locale::Locale(Settings *settings, QObject *parent)
    : QObject(parent), m_settings(settings)
{
    if (m_settings) {
        connect(m_settings, &Settings::languageChanged, this, &Locale::languageChanged);
    }
    buildTable();
}

QString Locale::language() const
{
    if (!m_settings) return QStringLiteral("ru");
    const QString l = m_settings->language();
    if (l == QStringLiteral("en") || l == QStringLiteral("ru")) return l;
    return QStringLiteral("ru");
}

QString Locale::t(const QString &key) const
{
    const auto it = m_table.constFind(key);
    if (it == m_table.constEnd()) return key;
    const QHash<QString, QString> &m = it.value();
    const QString l = language();
    const auto jt = m.constFind(l);
    if (jt != m.constEnd()) return jt.value();
    // Fall back to Russian, then key.
    const auto kt = m.constFind(QStringLiteral("ru"));
    return kt != m.constEnd() ? kt.value() : key;
}

QString Locale::t1(const QString &key, const QString &arg1) const
{
    return t(key).arg(arg1);
}

QString Locale::t2(const QString &key, const QString &arg1, const QString &arg2) const
{
    return t(key).arg(arg1, arg2);
}

void Locale::buildTable()
{
    auto &T = m_table;

    // --- Nav --------------------------------------------------------------
    put(T, "nav.home",     "Главная",     "Home");
    put(T, "nav.queue",    "Очередь",     "Queue");
    put(T, "nav.history",  "История",     "History");
    put(T, "nav.settings", "Настройки",   "Settings");
    put(T, "nav.about",    "О программе", "About");

    // --- Home -------------------------------------------------------------
    put(T, "home.title",        "Скачать видео",        "Download a video");
    put(T, "home.subtitle",     "Вставь ссылку — выбери формат — нажми «Скачать».",
                                "Paste a link, pick a format, hit «Download».");
    put(T, "home.urlPlaceholder", "Вставь URL видео…",   "Paste a video URL…");
    put(T, "home.format",        "Формат",              "Format");
    put(T, "home.quality",       "Качество",            "Quality");
    put(T, "home.outputDir",     "Папка сохранения",    "Output folder");
    put(T, "home.browse",        "Обзор…",              "Browse…");
    put(T, "home.download",      "Скачать",             "Download");
    put(T, "home.addToQueue",    "В очередь",           "Add to queue");
    put(T, "home.paste",         "Вставить",            "Paste");
    put(T, "home.clear",         "Очистить",            "Clear");
    put(T, "home.openFolder",    "Открыть папку",       "Open folder");

    // Quality / format labels (the values stay the same in both langs
    // because they are keys passed to yt-dlp; the *labels* are localised).
    put(T, "fmt.best",  "Максимум",                 "Best available");
    put(T, "fmt.2160",  "4K · 2160p",               "4K · 2160p");
    put(T, "fmt.1440",  "QHD · 1440p",              "QHD · 1440p");
    put(T, "fmt.1080",  "Full HD · 1080p",          "Full HD · 1080p");
    put(T, "fmt.720",   "HD · 720p",                "HD · 720p");
    put(T, "fmt.480",   "SD · 480p",                "SD · 480p");
    put(T, "fmt.360",   "Низкое · 360p",            "Low · 360p");
    put(T, "fmt.audio", "Только аудио",             "Audio only");

    // --- Queue / job status ----------------------------------------------
    put(T, "queue.empty",     "Очередь пуста",                           "The queue is empty");
    put(T, "queue.emptyHint", "Добавь видео со страницы «Главная».",     "Add a video from the «Home» page.");
    put(T, "queue.cancel",    "Отменить",                                "Cancel");
    put(T, "queue.retry",     "Повторить",                               "Retry");
    put(T, "queue.remove",    "Удалить из списка",                       "Remove from list");
    put(T, "queue.clearDone", "Убрать готовые",                          "Clear finished");
    put(T, "queue.clearFailed", "Убрать ошибки",                         "Clear errors");
    put(T, "queue.openFile",  "Показать в проводнике",                   "Show in folder");
    put(T, "queue.openLog",   "Лог",                                     "Log");

    put(T, "status.queued",         "В очереди",        "Queued");
    put(T, "status.resolving",      "Получаю инфо…",    "Fetching metadata…");
    put(T, "status.downloading",    "Скачивание",       "Downloading");
    put(T, "status.postprocessing", "Постобработка",    "Post-processing");
    put(T, "status.finished",       "Готово",           "Done");
    put(T, "status.failed",         "Ошибка",           "Failed");
    put(T, "status.canceled",       "Отменено",         "Canceled");

    // --- Settings ---------------------------------------------------------
    put(T, "set.title",          "Настройки",                  "Settings");
    put(T, "set.subtitle",       "Гибкая конфигурация загрузчика.", "Flexible downloader configuration.");
    put(T, "set.section.theme",  "Внешний вид",                "Appearance");
    put(T, "set.section.output", "Сохранение",                 "Output");
    put(T, "set.section.format", "Формат и качество",          "Format & quality");
    put(T, "set.section.extras", "Субтитры, обложки, метаданные", "Subtitles, thumbnails, metadata");
    put(T, "set.section.network","Сеть и параллелизм",         "Network & concurrency");
    put(T, "set.section.advanced","Дополнительно",             "Advanced");

    put(T, "set.theme",          "Тема",                       "Theme");
    put(T, "set.theme.snow",     "Белоснежная",                "Snow white");
    put(T, "set.theme.dark",     "Тёмная",                     "Dark");
    put(T, "set.theme.blackout", "Глубокий чёрный",            "Blackout");
    put(T, "set.compact",        "Компактный режим",           "Compact mode");
    put(T, "set.language",       "Язык интерфейса",            "Interface language");
    put(T, "set.language.ru",    "Русский",                    "Russian");
    put(T, "set.language.en",    "Английский",                 "English");

    put(T, "set.outputDir",      "Папка сохранения",                 "Output folder");
    put(T, "set.filenameTemplate","Шаблон имени файла",              "Filename template");
    put(T, "set.restrictFilenames","ASCII-имена (для FAT / сети)",   "ASCII-only names (FAT / network shares)");
    put(T, "set.organizeByUploader","Раскладывать по авторам",       "Organize by uploader");

    put(T, "set.defaultFormat",  "Качество по умолчанию",      "Default quality");
    put(T, "set.containerVideo", "Контейнер видео",            "Video container");
    put(T, "set.audioFormat",    "Аудио-формат",               "Audio format");
    put(T, "set.audioQuality",   "Аудио-битрейт (0 = лучший)", "Audio bitrate (0 = best)");
    put(T, "set.preferFreeFormats","Предпочитать свободные кодеки", "Prefer free codecs");

    put(T, "set.writeSubs",      "Скачивать субтитры",         "Download subtitles");
    put(T, "set.writeAutoSubs",  "Авто-субтитры",              "Auto-generated subtitles");
    put(T, "set.embedSubs",      "Встраивать субтитры",        "Embed subtitles");
    put(T, "set.subLangs",       "Языки субтитров",            "Subtitle languages");
    put(T, "set.writeThumbnail", "Сохранять обложку",          "Save thumbnail");
    put(T, "set.embedThumbnail", "Встраивать обложку",         "Embed thumbnail");
    put(T, "set.embedMetadata",  "Встраивать метаданные",      "Embed metadata");
    put(T, "set.embedChapters",  "Встраивать главы",           "Embed chapters");
    put(T, "set.sponsorblock",   "Вырезать спонсорские блоки", "Skip sponsor segments (SponsorBlock)");

    put(T, "set.maxConcurrent",  "Параллельных загрузок",      "Parallel downloads");
    put(T, "set.speedLimit",     "Лимит скорости (KB/s, 0 = без лимита)", "Speed limit (KB/s, 0 = unlimited)");
    put(T, "set.retries",        "Повторов при ошибке",        "Retries on error");
    put(T, "set.proxy",          "Прокси (URL)",               "Proxy URL");
    put(T, "set.cookiesBrowser", "Куки из браузера",           "Cookies from browser");
    put(T, "set.cookiesFile",    "Файл cookies (Netscape)",    "Cookies file (Netscape)");
    put(T, "set.userAgent",      "User-Agent",                 "User-Agent");

    put(T, "set.ytDlpPath",      "Путь к yt-dlp",              "Path to yt-dlp");
    put(T, "set.ffmpegPath",     "Путь к ffmpeg",              "Path to ffmpeg");
    put(T, "set.extraArgs",      "Доп. аргументы yt-dlp",      "Extra yt-dlp arguments");
    put(T, "set.reset",          "Сбросить настройки",         "Reset to defaults");
    put(T, "set.resetConfirm",   "Сбросить все настройки?",    "Reset all settings?");

    // --- About ------------------------------------------------------------
    put(T, "about.title",   "О программе",         "About");
    put(T, "about.tools",   "Внешние инструменты", "External tools");
    put(T, "about.notFound","Не найдено",          "Not found");
    put(T, "about.appDesc",
        "Volchay Downloader — настольный загрузчик видео с гибкой конфигурацией, очередью и тремя темами.",
        "Volchay Downloader — a desktop video grabber with flexible configuration, a queue and three themes.");

    // --- Toast messages ---------------------------------------------------
    put(T, "toast.added",       "Добавлено в очередь",      "Added to queue");
    put(T, "toast.copyPath",    "Путь скопирован",          "Path copied");
    put(T, "toast.invalidUrl",  "Неподдерживаемый URL",     "URL is not supported");
    put(T, "toast.noTools",     "yt-dlp не найден",         "yt-dlp not found");
    put(T, "toast.noOutputDir", "Папка сохранения не задана", "Output folder is not set");

    // --- Misc -------------------------------------------------------------
    put(T, "common.ok",     "OK",          "OK");
    put(T, "common.cancel", "Отмена",      "Cancel");
    put(T, "common.close",  "Закрыть",     "Close");
    put(T, "common.yes",    "Да",          "Yes");
    put(T, "common.no",     "Нет",         "No");
}
