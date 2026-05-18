#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDateTime>
#include <QMutex>
#include <QLibraryInfo>
#include <QQmlError>
#include <QSysInfo>
#include <QSurfaceFormat>
#include <QDebug>

#include <cstdio>
#include <cstdlib>
#include <exception>

#include "DownloadManager.h"
#include "DownloadQueueModel.h"
#include "Settings.h"
#include "Theme.h"
#include "ToolsLocator.h"
#include "AppLocale.h"

// ---------------------------------------------------------------------------
// File-based logger.
//
// Every qDebug / qInfo / qWarning / qCritical message goes both to stderr
// (visible when the .exe is launched from a console) and to a plain text
// log file at
//   Windows:  %LOCALAPPDATA%\Volchay\Volchay-Downloader\log.txt
//   Linux:    ~/.local/share/Volchay/Volchay-Downloader/log.txt
//
// The log is the user-facing way to report a "the app does not start"
// problem: it captures Qt plugin paths, QML errors and any context we
// managed to record before a crash.
// ---------------------------------------------------------------------------
namespace {

QFile  *g_logFile = nullptr;
QMutex  g_logMutex;
QString g_logPath;

const char *levelName(QtMsgType t)
{
    switch (t) {
        case QtDebugMsg:    return "DEBUG";
        case QtInfoMsg:     return "INFO ";
        case QtWarningMsg:  return "WARN ";
        case QtCriticalMsg: return "ERROR";
        case QtFatalMsg:    return "FATAL";
    }
    return "?    ";
}

void volchayMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    QMutexLocker lock(&g_logMutex);
    const QString ts   = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    const QString line = QStringLiteral("[%1] %2  %3")
                             .arg(ts, QLatin1String(levelName(type)), msg);

    const QByteArray bytes = line.toUtf8();
    if (g_logFile && g_logFile->isOpen()) {
        g_logFile->write(bytes);
        g_logFile->write("\n");
        if (type >= QtWarningMsg && ctx.file) {
            const QString where = QStringLiteral("        at %1:%2  (%3)")
                                      .arg(QString::fromUtf8(ctx.file))
                                      .arg(ctx.line)
                                      .arg(QString::fromUtf8(ctx.function ? ctx.function : ""));
            g_logFile->write(where.toUtf8());
            g_logFile->write("\n");
        }
        g_logFile->flush();
    }
    std::fputs(bytes.constData(), stderr);
    std::fputc('\n', stderr);
    std::fflush(stderr);

    if (type == QtFatalMsg) {
        if (g_logFile) { g_logFile->flush(); g_logFile->close(); }
        std::abort();
    }
}

void openLogFile(const QString &appDirHint)
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (dir.isEmpty()) dir = QDir::tempPath();
    QDir().mkpath(dir);

    g_logPath = QDir(dir).filePath(QStringLiteral("log.txt"));
    g_logFile = new QFile(g_logPath);

    // Rotate at ~2 MiB so the log never grows unbounded.
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append;
    if (g_logFile->exists() && g_logFile->size() > 2 * 1024 * 1024) {
        mode = QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate;
    }
    if (!g_logFile->open(mode)) {
        // Last-resort fallback: drop the log next to the executable.
        g_logPath = QDir(appDirHint).filePath(QStringLiteral("Volchay-Downloader.log"));
        g_logFile->setFileName(g_logPath);
        g_logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    }
}

void closeLogFile()
{
    QMutexLocker lock(&g_logMutex);
    if (g_logFile) {
        g_logFile->flush();
        g_logFile->close();
        delete g_logFile;
        g_logFile = nullptr;
    }
}

} // namespace

int main(int argc, char *argv[])
{
    QGuiApplication::setOrganizationName("Volchay");
    QGuiApplication::setApplicationName("Volchay-Downloader");
    QGuiApplication::setApplicationDisplayName("Volchay Downloader");
    QGuiApplication::setApplicationVersion("0.1.0");

    // The logger must be installed before any Qt object is constructed so
    // we capture plugin-load failures, QPA init crashes, etc.
    openLogFile(QFileInfo(QString::fromLocal8Bit(argv[0])).absolutePath());
    qInstallMessageHandler(volchayMessageHandler);

    qInfo().noquote() << "=== Volchay Downloader v0.1.0 starting ===";
    qInfo().noquote() << "Log file   :" << g_logPath;
    qInfo().noquote() << "Qt runtime :" << qVersion()
                      << "(built against" << QT_VERSION_STR << ")";
    qInfo().noquote() << "Host OS    :" << QSysInfo::prettyProductName()
                      << "/"  << QSysInfo::currentCpuArchitecture();
    qInfo().noquote() << "argv[0]    :" << QString::fromLocal8Bit(argv[0]);
    qInfo().noquote() << "Plugins    :" << QLibraryInfo::path(QLibraryInfo::PluginsPath);
    qInfo().noquote() << "QML import :" << QLibraryInfo::path(QLibraryInfo::QmlImportsPath);

    int rc = -1;
    try {
        // Pin a sane GL surface format up-front so the QPA backend honours
        // it on first window creation across drivers.
        QSurfaceFormat fmt;
        fmt.setDepthBufferSize(24);
        fmt.setStencilBufferSize(8);
        fmt.setSwapInterval(1);
        QSurfaceFormat::setDefaultFormat(fmt);

        QGuiApplication app(argc, argv);
        QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/app.png")));

        qInfo().noquote() << "App dir    :" << QCoreApplication::applicationDirPath();

        // Force Basic Qt Quick Controls style — we draw everything ourselves
        // (TitleBar / Sidebar / cards / buttons), so the native style would
        // only paint native chrome over the top of our acrylic surfaces.
        QQuickStyle::setStyle(QStringLiteral("Basic"));

        QFont base = app.font();
        base.setHintingPreference(QFont::PreferFullHinting);
        base.setStyleStrategy(QFont::PreferAntialias);
        app.setFont(base);

        qInfo() << "Constructing core services...";
        Settings           settings;
        Locale             locale(&settings);
        Theme              theme(&settings);
        ToolsLocator       tools;
        DownloadQueueModel queueModel;
        DownloadManager    manager(&settings, &tools, &queueModel);
        qInfo() << "Core services ready.";

        QQmlApplicationEngine engine;
        QObject::connect(&engine, &QQmlApplicationEngine::warnings,
                         &app, [](const QList<QQmlError> &warnings) {
            for (const QQmlError &w : warnings) {
                qWarning().noquote() << "QML warning:" << w.toString();
            }
        });
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                         &app, [](const QUrl &url) {
            qCritical().noquote() << "QML object creation failed for" << url.toString();
        });
#endif

        QQmlContext *ctx = engine.rootContext();
        ctx->setContextProperty(QStringLiteral("manager"),  &manager);
        ctx->setContextProperty(QStringLiteral("queue"),    &queueModel);
        ctx->setContextProperty(QStringLiteral("settings"), &settings);
        ctx->setContextProperty(QStringLiteral("theme"),    &theme);
        ctx->setContextProperty(QStringLiteral("tools"),    &tools);
        ctx->setContextProperty(QStringLiteral("i18n"),     &locale);

        // Re-evaluate every binding that touches i18n.t(...) when the user
        // changes language. QML cannot infer this dependency from a plain
        // Q_INVOKABLE call, so we briefly evict the context property and
        // re-install it, which hard-rebinds every expression referencing it.
        QObject::connect(&locale, &Locale::languageChanged,
                         &engine, [&engine, &locale]() {
            QQmlContext *c = engine.rootContext();
            c->setContextProperty(QStringLiteral("i18n"), QVariant());
            c->setContextProperty(QStringLiteral("i18n"), &locale);
            engine.retranslate();
        });

        qInfo() << "Loading qrc:/qml/Main.qml ...";
        engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
        if (engine.rootObjects().isEmpty()) {
            qCritical() << "QML root object list is empty — Main.qml failed to load.";
            closeLogFile();
            return -1;
        }
        qInfo() << "QML loaded, entering event loop.";
        rc = app.exec();
        qInfo() << "Event loop returned" << rc;
    } catch (const std::exception &e) {
        qCritical().noquote() << "Unhandled std::exception:" << e.what();
        closeLogFile();
        return -2;
    } catch (...) {
        qCritical() << "Unhandled non-std exception, aborting.";
        closeLogFile();
        return -3;
    }

    closeLogFile();
    return rc;
}
