/**
 * @file themeregistry.cpp
 * @brief Implementation of ThemeRegistry.
 */
#include "themeregistry.h"
#include "level.h"
#include "tripconditions.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QHash<QString, LevelTheme> ThemeRegistry::entries;
LevelTheme ThemeRegistry::defaultTheme;
LevelTheme ThemeRegistry::titleEntry;
bool ThemeRegistry::loaded = false;

namespace {

QString weatherKey(WeatherType w) {
    switch (w) {
        case WeatherType::COLD:  return "COLD";
        case WeatherType::HOT:   return "HOT";
        case WeatherType::RAINY: return "RAINY";
        case WeatherType::WINDY: return "WINDY";
        case WeatherType::SNOW:  return "SNOW";
        default:                 return "DEFAULT";
    }
}

QColor readColor(const QJsonObject& obj, const QString& key, const QColor& fallback) {
    if (!obj.contains(key)) return fallback;
    const QString hex = obj.value(key).toString();
    QColor c(hex);
    return c.isValid() ? c : fallback;
}

LevelTheme readTheme(const QJsonObject& obj) {
    LevelTheme t;
    t.slug           = obj.value("slug").toString();
    t.primary        = readColor(obj, "primary",       t.primary);
    t.secondary      = readColor(obj, "secondary",     t.secondary);
    t.textOnPrimary  = readColor(obj, "textOnPrimary", t.textOnPrimary);
    t.textOnSurface  = readColor(obj, "textOnSurface", t.textOnSurface);
    t.imagePath      = obj.value("image").toString();
    t.musicPath      = obj.value("music").toString();
    return t;
}

} // namespace

void ThemeRegistry::ensureLoaded() {
    if (!loaded) loadFromResource();
}

void ThemeRegistry::loadFromResource() {
    loaded = true;

    QFile f(":/data/themes.json");
    if (!f.open(QIODevice::ReadOnly)) return;

    const QByteArray raw = f.readAll();
    f.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return;

    const QJsonObject root = doc.object();

    // Default theme
    if (root.contains("default") && root.value("default").isObject())
        defaultTheme = readTheme(root.value("default").toObject());

    // Title screen theme
    if (root.contains("title") && root.value("title").isObject())
        titleEntry = readTheme(root.value("title").toObject());
    else
        titleEntry = defaultTheme;

    // Level themes
    const QJsonArray levels = root.value("levels").toArray();
    for (const QJsonValue& v : levels) {
        if (!v.isObject()) continue;
        const QJsonObject obj = v.toObject();
        const QString location = obj.value("location").toString();
        const QString weather  = obj.value("weather").toString();
        const QString key = location + "|" + weather;
        entries.insert(key, readTheme(obj));
    }
}

LevelTheme ThemeRegistry::themeFor(const Level& level) {
    ensureLoaded();
    const QString key = level.getLocationName() + "|" + weatherKey(level.getWeather());
    auto it = entries.find(key);
    if (it != entries.end()) return it.value();

    // Secondary fallback: try location-only match
    for (auto it2 = entries.begin(); it2 != entries.end(); ++it2) {
        if (it2.key().startsWith(level.getLocationName() + "|"))
            return it2.value();
    }
    return defaultTheme;
}

LevelTheme ThemeRegistry::titleTheme() {
    ensureLoaded();
    return titleEntry;
}
