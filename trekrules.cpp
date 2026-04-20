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
QJsonObject TrekRules::feedbackRoot;

void TrekRules::ensureLoaded()
{
    if (!rulesRoot.isEmpty() && !feedbackRoot.isEmpty()) return;

    QFile file(":/data/tuning.json");
    if (file.open(QIODevice::ReadOnly)) {
        rulesRoot = QJsonDocument::fromJson(file.readAll()).object();
    } else {
        qWarning() << "CRITICAL: Could not load tuning.json from resources!";
    }
    file.close();

    QFile feedbackFile(":/data/feedback.json");
    if (feedbackFile.open(QIODevice::ReadOnly)) {
        feedbackRoot = QJsonDocument::fromJson(feedbackFile.readAll()).object();
    } else {
        qWarning() << "CRITICAL: Could not load feedback.json from resources!";
    }
    feedbackFile.close();
}

double TrekRules::get(const QString& category, const QString& key)
{
    ensureLoaded();
    return rulesRoot.value(category).toObject().value(key).toDouble();
}

QString TrekRules::getTip(const QString& category, const QString& key)
{
    ensureLoaded();
    QJsonObject tipsObj = feedbackRoot.value("tips").toObject();
    QJsonObject catObj = tipsObj.value(category).toObject();
    if (catObj.contains(key)) {
        return catObj.value(key).toString();
    }
    return QString();
}

QStringList TrekRules::getEnvironmentalTips(WeatherType weather, TerrainType terrain,
                                            AdventureType adventure, int durationHours,
                                            bool skipGearCheck, int customCalories)
{
    QStringList tips;
    
    // 1. Hydration (Always include)
    QString weatherKey = weatherToString(weather);
    QString weatherTip = getTip("weather", weatherKey);
    if (!weatherTip.isEmpty()) tips << weatherTip;

    // 2. Nutrition & Energy (Only if not a water-only level)
    if (customCalories != 0) {
        QString foodTip;
        if (durationHours > 8) {
            foodTip = getTip("nutrition", "LONG");
        } else {
            foodTip = getTip("nutrition", "GENERAL");
        }
        if (!foodTip.isEmpty()) tips << foodTip;
    }

    // 3. Environmental & Gear (Advanced tips for non-beginner levels)
    if (!skipGearCheck) {
        QString itemWeatherTip = getTip("item_weather", weatherKey);
        if (!itemWeatherTip.isEmpty()) tips << itemWeatherTip;

        QString terrainKey = terrainToString(terrain);
        QString terrainTip = getTip("terrain", terrainKey);
        QString itemTerrainTip = getTip("item_terrain", terrainKey);
        if (!terrainTip.isEmpty()) tips << terrainTip;
        if (!itemTerrainTip.isEmpty()) tips << itemTerrainTip;

        if (adventure == AdventureType::BACKPACKING || adventure == AdventureType::CAMPING) {
            QString advTip = getTip("adventure", "OVERNIGHT");
            QString itemAdvTip = getTip("item_adventure", "OVERNIGHT");
            if (!advTip.isEmpty()) tips << advTip;
            if (!itemAdvTip.isEmpty()) tips << itemAdvTip;
        }

        if (durationHours > 8) {
            QString durTip = getTip("duration", "LONG");
            if (!durTip.isEmpty()) tips << durTip;
        }
    }

    return tips;
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

QString TrekRules::adventureToString(AdventureType a)
{
    switch(a) {
        case AdventureType::BACKPACKING: return "BACKPACKING";
        case AdventureType::CAMPING:     return "CAMPING";
        case AdventureType::DAY_HIKE:    return "DAY_HIKE";
        default: return "ALL";
    }
}
