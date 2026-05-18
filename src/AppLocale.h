#pragma once

#include <QObject>
#include <QHash>
#include <QString>
#include <QStringList>

class Settings;

// A tiny static translation table used by every QML file. We don't use
// Qt's tr() / QTranslator because the strings here are stored in a key
// → map-of-language layout, which is easier to maintain for a small
// app and lets the user switch language at runtime without re-loading
// .qm files.
//
// QML accesses everything through `i18n.t("ns.key")` — see main.cpp for
// the binding that forces every t() call to re-evaluate when the
// language changes.
class Locale : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language NOTIFY languageChanged)

public:
    explicit Locale(Settings *settings, QObject *parent = nullptr);

    QString language() const;

    // Look up a key. If unknown, returns the key itself so missing
    // strings are visible during development.
    Q_INVOKABLE QString t(const QString &key) const;
    Q_INVOKABLE QString t1(const QString &key, const QString &arg1) const;
    Q_INVOKABLE QString t2(const QString &key, const QString &arg1, const QString &arg2) const;

signals:
    void languageChanged();

private:
    void buildTable();

    Settings *m_settings = nullptr;
    QHash<QString, QHash<QString, QString>> m_table; // key -> lang -> text
};
