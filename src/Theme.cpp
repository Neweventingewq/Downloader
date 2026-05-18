#include "Theme.h"
#include "Settings.h"

Theme::Theme(Settings *settings, QObject *parent)
    : QObject(parent), m_settings(settings)
{
    if (m_settings) {
        connect(m_settings, &Settings::themeModeChanged, this, &Theme::paletteChanged);
        connect(m_settings, &Settings::compactChanged,   this, &Theme::compactChanged);
    }
}

QString Theme::mode() const
{
    return m_settings ? m_settings->themeMode() : QStringLiteral("dark");
}

bool Theme::compact() const
{
    return m_settings ? m_settings->compact() : false;
}

// ----- background / surface -----------------------------------------------

QColor Theme::bg() const
{
    if (isSnow())     return QColor("#FFFFFF");
    if (isBlackout()) return QColor("#000000");
    return QColor("#0E0D11");
}

QColor Theme::bgGradTop() const
{
    if (isSnow())     return QColor("#FFFFFF");
    if (isBlackout()) return QColor("#050505");
    return QColor("#15141A");
}

QColor Theme::bgGradBottom() const
{
    if (isSnow())     return QColor("#F2F2F2");
    if (isBlackout()) return QColor("#000000");
    return QColor("#08070A");
}

QColor Theme::sidebarBg() const
{
    if (isSnow())     return QColor("#F4F4F5");
    if (isBlackout()) return QColor::fromRgbF(1, 1, 1, 0.02);
    return QColor::fromRgbF(1, 1, 1, 0.025);
}

QColor Theme::surface() const
{
    if (isSnow())     return QColor("#EFEFF1");
    if (isBlackout()) return QColor::fromRgbF(1, 1, 1, 0.035);
    return QColor::fromRgbF(1, 1, 1, 0.045);
}

QColor Theme::surfaceHover() const
{
    if (isSnow())     return QColor("#E4E4E7");
    if (isBlackout()) return QColor::fromRgbF(1, 1, 1, 0.06);
    return QColor::fromRgbF(1, 1, 1, 0.075);
}

QColor Theme::border() const
{
    if (isSnow())     return QColor("#D4D4D8");
    if (isBlackout()) return QColor::fromRgbF(1, 1, 1, 0.06);
    return QColor::fromRgbF(1, 1, 1, 0.09);
}

QColor Theme::divider() const
{
    if (isSnow())     return QColor("#E4E4E7");
    if (isBlackout()) return QColor::fromRgbF(1, 1, 1, 0.04);
    return QColor::fromRgbF(1, 1, 1, 0.06);
}

// ----- accent --------------------------------------------------------------

QColor Theme::accent() const
{
    if (isSnow()) return amberSnow();
    return amber();
}

QColor Theme::accentHover() const
{
    return accent().lighter(115);
}

QColor Theme::accentDeep() const
{
    return accent().darker(125);
}

QColor Theme::accentSoft() const
{
    QColor a = accent();
    a.setAlphaF(isBlackout() ? 0.14f : 0.18f);
    return a;
}

QColor Theme::accentTextOn() const
{
    // The amber accent is bright enough that dark text reads best across
    // all three themes.
    return QColor("#1A1612");
}

// ----- text ---------------------------------------------------------------

QColor Theme::textPrimary() const
{
    if (isSnow())     return QColor("#3A3A3D");
    if (isBlackout()) return QColor("#F5F4F0");
    return QColor("#F2F1ED");
}

QColor Theme::textSecondary() const
{
    if (isSnow())     return QColor("#6E6E72");
    if (isBlackout()) return QColor("#9C968C");
    return QColor("#A8A39A");
}

QColor Theme::textMuted() const
{
    if (isSnow())     return QColor("#9C9CA0");
    if (isBlackout()) return QColor("#5E5A52");
    return QColor("#6F6B63");
}
