#include "AppLocale.h"
#include "Settings.h"

namespace {
// put(): the canonical helper for adding a translated string.  We list
// English first because it is the application's *primary* / fallback
// language — `Locale::t()` will use the English text whenever the
// current language is unknown or a key is missing a translation.
//
// Ukrainian is the second supported language; adding a third one is a
// matter of widening this helper and the `Locale::language()` switch.
void put(QHash<QString, QHash<QString, QString>> &t,
         const QString &key,
         const QString &en, const QString &uk)
{
    QHash<QString, QString> m;
    m.insert(QStringLiteral("en"), en);
    m.insert(QStringLiteral("uk"), uk);
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
    if (!m_settings) return QStringLiteral("en");
    const QString l = m_settings->language();
    // Only the languages we actually ship translations for.  Anything
    // else (legacy "ru" from older Settings storage, a future locale we
    // haven't bundled yet) falls back to English so the UI never shows
    // empty strings.
    if (l == QStringLiteral("en") || l == QStringLiteral("uk")) return l;
    return QStringLiteral("en");
}

QString Locale::t(const QString &key) const
{
    const auto it = m_table.constFind(key);
    if (it == m_table.constEnd()) return key;
    const QHash<QString, QString> &m = it.value();
    const QString l = language();
    const auto jt = m.constFind(l);
    if (jt != m.constEnd()) return jt.value();
    // Fall back to English, then key.
    const auto kt = m.constFind(QStringLiteral("en"));
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
    put(T, "nav.home",     "Home",         "Головна");
    put(T, "nav.queue",    "Queue",        "Черга");
    put(T, "nav.history",  "History",      "Історія");
    put(T, "nav.settings", "Settings",     "Налаштування");
    put(T, "nav.about",    "About",        "Про програму");

    // --- Home -------------------------------------------------------------
    put(T, "home.title",        "Download a video",       "Завантажити відео");
    put(T, "home.subtitle",     "Paste a link, pick a format, hit «Download».",
                                "Встав посилання — обери формат — натисни «Завантажити».");
    put(T, "home.urlPlaceholder", "Paste a video URL…",   "Встав URL відео…");
    put(T, "home.format",        "Format",                "Формат");
    put(T, "home.quality",       "Quality",               "Якість");
    put(T, "home.outputDir",     "Output folder",         "Папка збереження");
    put(T, "home.browse",        "Browse…",               "Огляд…");
    put(T, "home.download",      "Download",              "Завантажити");
    put(T, "home.addToQueue",    "Add to queue",          "У чергу");
    put(T, "home.paste",         "Paste",                 "Вставити");
    put(T, "home.clear",         "Clear",                 "Очистити");
    put(T, "home.openFolder",    "Open folder",           "Відкрити папку");

    // Quality / format labels (the values stay the same in both langs
    // because they are keys passed to yt-dlp; the *labels* are localised).
    put(T, "fmt.best",  "Best available",             "Максимум");
    put(T, "fmt.2160",  "4K · 2160p",                 "4K · 2160p");
    put(T, "fmt.1440",  "QHD · 1440p",                "QHD · 1440p");
    put(T, "fmt.1080",  "Full HD · 1080p",            "Full HD · 1080p");
    put(T, "fmt.720",   "HD · 720p",                  "HD · 720p");
    put(T, "fmt.480",   "SD · 480p",                  "SD · 480p");
    put(T, "fmt.360",   "Low · 360p",                 "Низька · 360p");
    put(T, "fmt.audio", "Audio only",                 "Тільки аудіо");

    // --- Queue / job status ----------------------------------------------
    put(T, "queue.empty",     "The queue is empty",                       "Черга порожня");
    put(T, "queue.emptyHint", "Add a video from the «Home» page.",        "Додай відео зі сторінки «Головна».");
    put(T, "queue.cancel",    "Cancel",                                    "Скасувати");
    put(T, "queue.retry",     "Retry",                                     "Повторити");
    put(T, "queue.remove",    "Remove from list",                          "Видалити зі списку");
    put(T, "queue.clearDone", "Clear finished",                            "Прибрати готові");
    put(T, "queue.clearFailed", "Clear errors",                            "Прибрати помилки");
    put(T, "queue.openFile",  "Show in folder",                            "Показати в провіднику");
    put(T, "queue.openLog",   "Log",                                       "Лог");

    put(T, "status.queued",         "Queued",            "У черзі");
    put(T, "status.resolving",      "Fetching metadata…","Отримую інфо…");
    put(T, "status.downloading",    "Downloading",       "Завантаження");
    put(T, "status.postprocessing", "Post-processing",   "Постобробка");
    put(T, "status.finished",       "Done",              "Готово");
    put(T, "status.failed",         "Failed",            "Помилка");
    put(T, "status.canceled",       "Canceled",          "Скасовано");

    // --- Recognised yt-dlp error categories ------------------------------
    // These are emitted by DownloadManager::parseStderrLine when it sees
    // a specific failure signature it can give the user a better
    // explanation for than yt-dlp's raw English warning text.
    put(T, "error.cookiesLocked",
        "The browser is holding its cookie database locked. Close Chrome / Edge / Brave "
        "fully (including background tray helpers) or switch to Firefox.",
        "Браузер тримає базу cookies заблокованою. Закрий Chrome / Edge / Brave повністю "
        "(включно з фоновими процесами в треї) або переключись на Firefox.");
    // Chrome 127+ (August 2024) rolled out App-Bound Encryption (ABE): the
    // master cookie key is wrapped with a DPAPI blob that can only be
    // unwrapped by the elevated `elevation_service.exe`, so any process
    // that isn't Chrome itself — including yt-dlp — fails to decrypt the
    // sqlite contents.  There is no software-side workaround on our end;
    // the user has to either dump cookies to a Netscape file (e.g. via a
    // browser extension) or switch to Firefox / Safari.  See yt-dlp
    // issue 10927 for the upstream tracker.
    put(T, "error.cookiesDpapi",
        "Chrome 127+ encrypts cookies with an app-bound key that yt-dlp cannot decrypt. "
        "Export cookies to a file (e.g. via the «Get cookies.txt LOCALLY» extension) "
        "and set it in the *Cookies file* field below, or switch to Firefox.",
        "Chrome 127+ шифрує cookies app-bound ключем, який yt-dlp не може розшифрувати. "
        "Експортуй cookies у файл (наприклад, розширенням «Get cookies.txt LOCALLY») "
        "і вкажи його в полі «Cookies file» нижче, або переключись на Firefox.");
    // YouTube's anti-bot challenge: identical recommendation to the
    // cookie failures because in 99% of cases that's exactly what
    // happened — cookies were either missing or got dropped silently
    // by the DPAPI failure above.  We keep the message short and
    // action-oriented; the full background is in Settings → Cookies
    // from browser hint.
    put(T, "error.youtubeBotCheck",
        "YouTube asked yt-dlp to confirm it's not a bot. Pass cookies: either export them "
        "from your browser to a file (e.g. «Get cookies.txt LOCALLY» extension) and pick it "
        "in *Settings → Cookies file*, or set *Cookies from browser* to firefox.",
        "YouTube попросив yt-dlp підтвердити, що це не бот. Передай cookies: або експортуй їх "
        "із браузера у файл (наприклад, розширенням «Get cookies.txt LOCALLY») і вкажи його в "
        "*Налаштування → Cookies file*, або встанови *Cookies з браузера* на firefox.");
    // No format matched our -f chain.  resolveFormatSpec() now always
    // appends a `b/best` safety net, so this should be rare — but
    // live streams, premieres and members-only manifests can still
    // hit it.  The actionable suggestion is to widen the quality
    // setting and turn off the free-codecs filter.
    put(T, "error.formatNotAvailable",
        "yt-dlp could not find a downloadable format for this video. Try *Settings → Default quality = "
        "Best available* and turn off *Prefer free codecs*; for live streams / premieres this may simply "
        "mean the stream isn't accessible to non-members.",
        "yt-dlp не знайшов формату для завантаження цього відео. Спробуй *Налаштування → Якість за "
        "замовчуванням = Максимум* і вимкни *Надавати перевагу вільним кодекам*; для трансляцій / "
        "прем'єр це може означати, що потік просто недоступний без членства.");

    // --- Settings ---------------------------------------------------------
    put(T, "set.title",          "Settings",                   "Налаштування");
    put(T, "set.subtitle",       "Flexible downloader configuration.", "Гнучка конфігурація завантажувача.");
    put(T, "set.section.theme",  "Appearance",                 "Зовнішній вигляд");
    put(T, "set.section.output", "Output",                     "Збереження");
    put(T, "set.section.format", "Format & quality",           "Формат і якість");
    put(T, "set.section.extras", "Subtitles, thumbnails, metadata", "Субтитри, обкладинки, метадані");
    put(T, "set.section.network","Network & concurrency",      "Мережа й паралелізм");
    put(T, "set.section.advanced","Advanced",                  "Додатково");

    put(T, "set.theme",          "Theme",                      "Тема");
    put(T, "set.theme.snow",     "Snow white",                 "Білосніжна");
    put(T, "set.theme.dark",     "Dark",                       "Темна");
    put(T, "set.theme.blackout", "Blackout",                   "Глибокий чорний");
    put(T, "set.compact",        "Compact mode",               "Компактний режим");
    put(T, "set.language",       "Interface language",         "Мова інтерфейсу");
    put(T, "set.language.en",    "English",                    "Англійська");
    put(T, "set.language.uk",    "Ukrainian",                  "Українська");

    put(T, "set.outputDir",      "Output folder",              "Папка збереження");
    put(T, "set.filenameTemplate","Filename template",         "Шаблон імені файла");
    put(T, "set.restrictFilenames","ASCII-only names (FAT / network shares)", "ASCII-імена (для FAT / мережі)");
    put(T, "set.organizeByUploader","Organize by uploader",    "Розкладати за авторами");

    put(T, "set.defaultFormat",  "Default quality",            "Якість за замовчуванням");
    put(T, "set.containerVideo", "Video container",            "Контейнер відео");
    // MP4 hint: explain why MP4 is the safe default and what the
    // app does about the YouTube-Opus-in-MP4 footgun.
    put(T, "set.containerVideo.mp4.hint",
        "Recommended: plays in every player, including Windows' built-in Films & TV. "
        "yt-dlp is told to prefer AAC audio for MP4 output so the file plays out of the box.",
        "Рекомендовано: програється у будь-якому плеєрі, зокрема у вбудованому «Кіно і ТБ» Windows. "
        "yt-dlp націлено на AAC-аудіо для MP4, щоб файл відкривався відразу.");
    put(T, "set.containerVideo.other.hint",
        "Better quality (keeps YouTube's original Opus / VP9 / AV1 streams without re-mixing), "
        "but Windows' built-in player won't open it — use VLC / mpv / Chrome / a modern media player.",
        "Краща якість (зберігає оригінальні Opus / VP9 / AV1 потоки YouTube без перепаковки), "
        "але вбудований плеєр Windows такий файл не відкриє — використовуй VLC / mpv / Chrome / сучасний плеєр.");
    put(T, "set.audioFormat",    "Audio format",               "Аудіо-формат");
    put(T, "set.audioFormat.hint",
        "Only used for audio-only downloads (the *Audio* quick action). "
        "For video downloads the app keeps YouTube's best audio stream as-is and merges it into the chosen container.",
        "Використовується лише для аудіо-завантажень (швидка дія *Аудіо*). "
        "Для відео-завантажень додаток залишає найкращий аудіо-потік YouTube без змін і кладе його в обраний контейнер.");
    put(T, "set.audioQuality",   "Audio bitrate (0 = best)",   "Аудіо-бітрейт (0 = найкращий)");
    put(T, "set.preferFreeFormats","Prefer free codecs",       "Надавати перевагу вільним кодекам");

    put(T, "set.writeSubs",      "Download subtitles",         "Завантажувати субтитри");
    put(T, "set.writeAutoSubs",  "Auto-generated subtitles",   "Авто-субтитри");
    put(T, "set.embedSubs",      "Embed subtitles",            "Вбудовувати субтитри");
    put(T, "set.subLangs",       "Subtitle languages",         "Мови субтитрів");
    put(T, "set.writeThumbnail", "Save thumbnail",             "Зберігати обкладинку");
    put(T, "set.embedThumbnail", "Embed thumbnail",            "Вбудовувати обкладинку");
    put(T, "set.embedMetadata",  "Embed metadata",             "Вбудовувати метадані");
    put(T, "set.embedChapters",  "Embed chapters",             "Вбудовувати розділи");
    put(T, "set.sponsorblock",   "Skip sponsor segments (SponsorBlock)", "Вирізати спонсорські блоки");

    put(T, "set.maxConcurrent",  "Parallel downloads",         "Паралельних завантажень");
    put(T, "set.speedLimit",     "Speed limit (KB/s, 0 = unlimited)", "Ліміт швидкості (KB/s, 0 = без обмежень)");
    put(T, "set.retries",        "Retries on error",           "Повторів при помилці");
    put(T, "set.proxy",          "Proxy URL",                  "Проксі (URL)");
    put(T, "set.cookiesBrowser", "Cookies from browser",       "Cookies з браузера");
    // The hint covers both failure modes for Chromium browsers: the
    // running-process file lock (Cookies.sqlite held with a brief
    // exclusive lock during write transactions) and, much more sticky,
    // Chrome 127+'s App-Bound Encryption that yt-dlp simply cannot
    // unwrap.  Closing the browser fixes (1) but not (2).
    put(T, "set.cookiesBrowser.hint",
        "Chrome / Edge / Brave / Opera / Vivaldi lock their cookie database while running, and Chrome 127+ "
        "also encrypts cookies with an app-bound key that yt-dlp cannot decrypt. "
        "Closing the browser helps with the lock but not with the encryption — for Chrome 127+ "
        "either export cookies to a Netscape file (use a browser extension like «Get cookies.txt LOCALLY») "
        "and point the «Cookies file» field below at it, or switch to Firefox.",
        "Chrome / Edge / Brave / Opera / Vivaldi блокують свою базу cookies, поки запущені, а Chrome 127+ "
        "ще й шифрує cookies app-bound ключем, який yt-dlp розшифрувати не може. "
        "Закриття браузера лікує блокування, але не шифрування — для Chrome 127+ "
        "або експортуй cookies у Netscape-файл (наприклад, розширенням «Get cookies.txt LOCALLY») "
        "та вкажи його в полі «Cookies file» нижче, або переключись на Firefox.");
    put(T, "set.cookiesFile",    "Cookies file (Netscape)",    "Файл cookies (Netscape)");
    put(T, "set.userAgent",      "User-Agent",                 "User-Agent");
    put(T, "set.politeSleep",    "Polite-mode pause (sec)",    "Пауза «ввічливого режиму» (сек)");
    put(T, "set.politeSleep.hint",
        "Seconds yt-dlp waits between HTTP requests. A small pause (3 sec is plenty) "
        "is enough to keep YouTube's anti-bot heuristics off a logged-in account, "
        "especially important if you own a large channel.",
        "Секунди, які yt-dlp чекає між HTTP-запитами. Невелика пауза (достатньо 3 сек) "
        "тримає твій акаунт поза антибот-евристикою YouTube — особливо важливо, якщо "
        "у тебе великий канал.");

    // --- Sign-in card ------------------------------------------------------
    put(T, "set.section.signin", "YouTube sign-in",          "Вхід у YouTube");
    put(T, "signin.title",       "Sign in to YouTube",       "Увійти у YouTube");
    put(T, "signin.youtube",     "YouTube account",          "Акаунт YouTube");
    put(T, "signin.warning",
        "Do not use your main Google account. Create a separate account for downloads "
        "(accounts.google.com/signup) — that way if YouTube ever flags this session, "
        "you'll only lose a throwaway account, not your channel.",
        "Не використовуй основний акаунт Google. Заведи окремий під завантаження "
        "(accounts.google.com/signup) — якщо YouTube колись помітить цю сесію, "
        "втратиш одноразовий акаунт, а не свій канал.");
    put(T, "signin.hint",
        "Once signed in, the app reuses these cookies for every YouTube download — no "
        "exporting files, no Chrome closing. Cookies are stored only inside this app's "
        "data folder and are isolated from your system browsers.",
        "Після входу додаток використовуватиме ці cookies для кожного завантаження "
        "YouTube — без експорту файлів, без закриття Chrome. Cookies зберігаються лише "
        "в папці даних цього додатка і не пов'язані з твоїми системними браузерами.");
    put(T, "signin.status.signedin",    "Signed in",                 "Підключено");
    put(T, "signin.status.notsignedin", "Not signed in",             "Не підключено");
    put(T, "signin.button.signin",      "Sign in",                   "Увійти");
    put(T, "signin.button.signinAgain", "Sign in again",             "Увійти ще раз");
    put(T, "signin.button.signout",     "Sign out",                  "Вийти");
    put(T, "signin.button.close",       "Close",                     "Закрити");
    put(T, "signin.window.signedin",    "Signed in — you can close this window.",
                                         "Підключено — це вікно можна закрити.");
    put(T, "signin.window.notyet",      "Log in with your secondary Google account.",
                                         "Увійди вторинним акаунтом Google.");

    put(T, "set.ytDlpPath",      "Path to yt-dlp",             "Шлях до yt-dlp");
    put(T, "set.ffmpegPath",     "Path to ffmpeg",             "Шлях до ffmpeg");
    put(T, "set.extraArgs",      "Extra yt-dlp arguments",     "Дод. аргументи yt-dlp");
    put(T, "set.reset",          "Reset to defaults",          "Скинути налаштування");
    put(T, "set.resetConfirm",   "Reset all settings?",        "Скинути всі налаштування?");

    // --- About ------------------------------------------------------------
    put(T, "about.title",   "About",                  "Про програму");
    put(T, "about.tools",   "External tools",         "Зовнішні інструменти");
    put(T, "about.notFound","Not found",              "Не знайдено");
    put(T, "about.appDesc",
        "Volchay Downloader — a desktop video grabber with flexible configuration, a queue and three themes.",
        "Volchay Downloader — настільний завантажувач відео з гнучкою конфігурацією, чергою й трьома темами.");

    // --- Toast messages ---------------------------------------------------
    put(T, "toast.added",       "Added to queue",          "Додано в чергу");
    put(T, "toast.copyPath",    "Path copied",             "Шлях скопійовано");
    put(T, "toast.invalidUrl",  "URL is not supported",    "Непідтримуваний URL");
    put(T, "toast.noTools",     "yt-dlp not found",        "yt-dlp не знайдено");
    put(T, "toast.noOutputDir", "Output folder is not set", "Папку збереження не задано");

    // --- Misc -------------------------------------------------------------
    put(T, "common.ok",     "OK",          "OK");
    put(T, "common.cancel", "Cancel",      "Скасувати");
    put(T, "common.close",  "Close",       "Закрити");
    put(T, "common.yes",    "Yes",         "Так");
    put(T, "common.no",     "No",          "Ні");
}
