/**
 * @file themeregistry.h
 * @brief Loads per-level themes from data/themes.json.
 */
#ifndef THEMEREGISTRY_H
#define THEMEREGISTRY_H

#include <QHash>
#include <QString>
#include "theme.h"

class Level;

class ThemeRegistry {
public:
    static void ensureLoaded();

    /**
     * @brief Returns the theme for a given level.
     *
     * Keyed by "<locationName>|<weather>" so the two Olympic levels can
     * be distinguished. Falls back to a neutral default theme if no
     * entry matches.
     */
    static LevelTheme themeFor(const Level& level);

    /**
     * @brief Returns the title-screen theme.
     */
    static LevelTheme titleTheme();

private:
    static void loadFromResource();
    static QHash<QString, LevelTheme> entries;
    static LevelTheme defaultTheme;
    static LevelTheme titleEntry;
    static bool loaded;
};

#endif // THEMEREGISTRY_H
