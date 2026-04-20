/**
 * @file theme.h
 * @brief Per-level visual + audio theme descriptor.
 */
#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QString>

struct LevelTheme {
    QString slug;
    QColor  accent    = QColor("#2E4A3F");
    QString imagePath;
    QString musicPath;
};

#endif // THEME_H
