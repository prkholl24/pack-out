/**
 * @file trekregistry.h
 * @brief Factory for loading and managing backpacking levels.
 */

#ifndef TREKREGISTRY_H
#define TREKREGISTRY_H

#include <vector>
#include <QJsonObject>
#include "level.h"

/**
 * @brief Responsible for loading trek definitions from external JSON data.
 * 
 * Centralizes the creation of Level objects, keeping the UI layer 
 * decoupled from the specific level configurations.
 */
class TrekRegistry
{
public:
    /**
     * @brief Loads all treks from the compiled resources.
     * @return A vector of initialized Level objects.
     */
    static std::vector<Level> loadTreks();

private:
    /**
     * @brief Translates a single JSON entry into a TripRequirements struct.
     * @param obj - The JSON object for one trek.
     */
    static TripRequirements parseTrip(const QJsonObject& obj);

    // Enum Conversion Helpers
    static TerrainType stringToTerrain(const QString& s);
    static WeatherType stringToWeather(const QString& s);
    static AdventureType stringToAdventure(const QString& s);
};

#endif // TREKREGISTRY_H
