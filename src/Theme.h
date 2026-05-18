#pragma once

#include <QObject>
#include <QColor>
#include <QString>

class Settings;

// Application-wide theme system. Exposed to QML as the context property
// `theme`. The theme follows `settings.themeMode`, one of:
//   - "snow"     — pure white floor with neutral grays + amber accent
//   - "dark"     — graphite floor with amber accent (default)
//   - "blackout" — pure #000000 floor for OLED / matte black setups
//
// The amber accent is shared between dark and blackout so the brand
// stays consistent; snow uses a slightly deeper amber for contrast
// against white surfaces.
class Theme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString mode  READ mode  NOTIFY paletteChanged)
    Q_PROPERTY(bool dark     READ isDark  NOTIFY paletteChanged)
    Q_PROPERTY(bool snow     READ isSnow  NOTIFY paletteChanged)
    Q_PROPERTY(bool blackout READ isBlackout NOTIFY paletteChanged)

    Q_PROPERTY(QColor bg            READ bg            NOTIFY paletteChanged)
    Q_PROPERTY(QColor bgGradTop     READ bgGradTop     NOTIFY paletteChanged)
    Q_PROPERTY(QColor bgGradBottom  READ bgGradBottom  NOTIFY paletteChanged)
    Q_PROPERTY(QColor sidebarBg     READ sidebarBg     NOTIFY paletteChanged)
    Q_PROPERTY(QColor surface       READ surface       NOTIFY paletteChanged)
    Q_PROPERTY(QColor surfaceHover  READ surfaceHover  NOTIFY paletteChanged)
    Q_PROPERTY(QColor border        READ border        NOTIFY paletteChanged)
    Q_PROPERTY(QColor divider       READ divider       NOTIFY paletteChanged)

    Q_PROPERTY(QColor accent        READ accent        NOTIFY paletteChanged)
    Q_PROPERTY(QColor accentHover   READ accentHover   NOTIFY paletteChanged)
    Q_PROPERTY(QColor accentDeep    READ accentDeep    NOTIFY paletteChanged)
    Q_PROPERTY(QColor accentSoft    READ accentSoft    NOTIFY paletteChanged)
    Q_PROPERTY(QColor accentTextOn  READ accentTextOn  NOTIFY paletteChanged)

    Q_PROPERTY(QColor textPrimary   READ textPrimary   NOTIFY paletteChanged)
    Q_PROPERTY(QColor textSecondary READ textSecondary NOTIFY paletteChanged)
    Q_PROPERTY(QColor textMuted     READ textMuted     NOTIFY paletteChanged)

    Q_PROPERTY(QColor success READ success CONSTANT)
    Q_PROPERTY(QColor warning READ warning CONSTANT)
    Q_PROPERTY(QColor error   READ error   CONSTANT)

    Q_PROPERTY(bool   compact READ compact NOTIFY compactChanged)

public:
    explicit Theme(Settings *settings, QObject *parent = nullptr);

    QString mode() const;
    bool isDark() const     { return mode() == QStringLiteral("dark"); }
    bool isSnow() const     { return mode() == QStringLiteral("snow"); }
    bool isBlackout() const { return mode() == QStringLiteral("blackout"); }
    bool compact() const;

    QColor bg() const;
    QColor bgGradTop() const;
    QColor bgGradBottom() const;
    QColor sidebarBg() const;
    QColor surface() const;
    QColor surfaceHover() const;
    QColor border() const;
    QColor divider() const;

    QColor accent() const;
    QColor accentHover() const;
    QColor accentDeep() const;
    QColor accentSoft() const;
    QColor accentTextOn() const;

    QColor textPrimary() const;
    QColor textSecondary() const;
    QColor textMuted() const;

    QColor success() const { return QColor("#22C55E"); }
    QColor warning() const { return QColor("#F59E0B"); }
    QColor error() const   { return QColor("#F43F5E"); }

signals:
    void paletteChanged();
    void compactChanged();

private:
    QColor amber() const { return QColor(245, 158, 11); }     // #F59E0B
    QColor amberSnow() const { return QColor(217, 119, 6); }  // #D97706 (slightly deeper for white floors)

    Settings *m_settings = nullptr;
};
