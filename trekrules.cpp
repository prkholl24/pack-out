/**
 * @file trekrules.cpp
 * @brief Implementation of the TrekRules manager.
 */

#include "trekrules.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

QJsonObject TrekRules::rulesRoot;

void TrekRules::ensureLoaded()
{
    if (!rulesRoot.isEmpty()) return;

    QFile file(":/data/tuning.json");
    if (file.open(QIODevice::ReadOnly)) {
        rulesRoot = QJsonDocument::fromJson(file.readAll()).object();
    } else {
        qWarning() << "CRITICAL: Could not load tuning.json from resources!";
    }
}

double TrekRules::get(const QString& category, const QString& key)
{
    ensureLoaded();
    return rulesRoot.value(category).toObject().value(key).toDouble();
}

RuleModifier TrekRules::getTerrainModifier(TerrainType terrain)
{
    ensureLoaded();
    QString key = terrainToString(terrain);
    QJsonObject obj = rulesRoot.value("terrain_multipliers").toObject().value(key).toObject();
    return { obj.value("energy").toDouble(1.0), obj.value("water").toDouble(1.0) };
}

RuleModifier TrekRules::getWeatherModifier(WeatherType weather)
{
    ensureLoaded();
    QString key = weatherToString(weather);
    QJsonObject obj = rulesRoot.value("weather_multipliers").toObject().value(key).toObject();
    return { obj.value("energy").toDouble(1.0), obj.value("water").toDouble(1.0) };
}

QStringList TrekRules::getWeatherKeywords(WeatherType weather)
{
    ensureLoaded();
    QJsonObject keywords = rulesRoot.value("weather_keywords").toObject();
    QString key = weatherToString(weather);

    QStringList list;
    QJsonArray arr = keywords.value(key).toArray();
    for (int i = 0; i < arr.size(); ++i) list << arr[i].toString();
    return list;
}

QString TrekRules::terrainToString(TerrainType t)
{
    switch(t) {
        case TerrainType::DESERT:   return "DESERT";
        case TerrainType::MOUNTAIN: return "MOUNTAIN";
        case TerrainType::ROCKY:    return "ROCKY";
        case TerrainType::FOREST:   return "FOREST";
        default: return "ALL";
    }
}

QString TrekRules::weatherToString(WeatherType w)
{
    switch(w) {
        case WeatherType::SNOW:  return "SNOW";
        case WeatherType::WINDY: return "WINDY";
        case WeatherType::HOT:   return "HOT";
        case WeatherType::COLD:  return "COLD";
        case WeatherType::RAINY: return "RAINY";
        default: return "DEFAULT";
    }
}
