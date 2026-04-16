/**
 * @file trekregistry.cpp
 * @brief Implementation of the TrekRegistry class.
 */

#include "trekregistry.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <utility>

std::vector<Level> TrekRegistry::loadTreks()
{
    std::vector<Level> treks;

    QFile file(":/data/treks.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load treks.json from resources.";
        return treks;
    }

    QJsonArray root = QJsonDocument::fromJson(file.readAll()).array();

    for (int i = 0; i < root.size(); ++i) {
        treks.push_back(Level(parseTrip(root[i].toObject())));
    }

    return treks;
}

TripRequirements TrekRegistry::parseTrip(const QJsonObject& obj)
{
    TripRequirements req;
    req.locationName = obj.value("name").toString();
    req.description = obj.value("description").toString();
    req.miles = obj.value("miles").toDouble();
    req.elevationGain = obj.value("elevationGain").toDouble();
    req.durationHours = obj.value("duration").toInt();
    
    // Map JSON strings to internal logic Enums
    req.terrain = stringToTerrain(obj.value("terrain").toString());
    req.weather = stringToWeather(obj.value("weather").toString());
    req.adventure = stringToAdventure(obj.value("adventure").toString());
    req.scienceFocus = obj.value("scienceFocus").toString();

    // Optional overrides: 0 = water-only level, -1 (default) = auto-calculate
    if (obj.contains("customCalories"))
        req.customCalories = obj.value("customCalories").toInt(-1);
    if (obj.contains("customHydration"))
        req.customHydration = obj.value("customHydration").toDouble(-1);
    if (obj.contains("packVolumeL"))
        req.packVolumeL = obj.value("packVolumeL").toDouble(20.0);

    req.skipGearCheck = obj.value("skipGearCheck").toBool(false);

    for (const QJsonValue& v : obj.value("availableItems").toArray())
        req.availableItems << v.toString();

    return req;
}

TerrainType TrekRegistry::stringToTerrain(const QString& s)
{
    if (s == "DESERT")   return TerrainType::DESERT;
    if (s == "MOUNTAIN") return TerrainType::MOUNTAIN;
    if (s == "ROCKY")    return TerrainType::ROCKY;
    if (s == "FOREST")   return TerrainType::FOREST;
    return TerrainType::ALL;
}

WeatherType TrekRegistry::stringToWeather(const QString& s)
{
    if (s == "COLD")  return WeatherType::COLD;
    if (s == "HOT")   return WeatherType::HOT;
    if (s == "RAINY") return WeatherType::RAINY;
    if (s == "WINDY") return WeatherType::WINDY;
    if (s == "SNOW")  return WeatherType::SNOW;
    return WeatherType::DEFAULT;
}

AdventureType TrekRegistry::stringToAdventure(const QString& s)
{
    if (s == "BACKPACKING") return AdventureType::BACKPACKING;
    if (s == "CAMPING")     return AdventureType::CAMPING;
    if (s == "DAY_HIKE")    return AdventureType::DAY_HIKE;
    return AdventureType::ALL;
}
