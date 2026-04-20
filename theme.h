/**
 * @file theme.h
 * @brief Per-level visual + audio theme descriptor.
 *
 * Bundles the colors, location photo, and music track associated with a
 * single level. Populated by ThemeRegistry from data/themes.json and
 * applied to the views (BriefView, DebriefView, BackpackScene) as the
 * player moves between levels.
 */
#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QString>

struct LevelTheme {
    QString slug;
    QColor primary        = QColor("#2E4A3F");
    QColor secondary      = QColor("#C8D9B8");
    QColor textOnPrimary  = QColor("#1E1E1E");
    QColor textOnSurface  = QColor("#1E1E1E");
    QString imagePath;
    QString musicPath;
};

#endif // THEME_H
