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

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();

    // Use std::as_const to prevent detachment of the QJsonObject
    for (auto it = std::as_const(root).begin(); it != std::as_const(root).end(); ++it) {
        treks.push_back(Level(parseTrip(it.value().toObject())));
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
