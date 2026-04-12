#ifndef TRIPCONDITIONS_H
#define TRIPCONDITIONS_H

#include <QString>

enum class WeatherType {
    COLD,
    HOT,
    RAINY,
    WINDY,
    SNOW,
    DEFAULT
};

enum class TerrainType {
    DESERT,
    FOREST,
    MOUNTAIN,
    ROCKY,
    ALL
};

enum class AdventureType {
    BACKPACKING,
    CAMPING,
    DAY_HIKE,
    ALL
};

/**
 * @brief Groups all physical and environmental parameters for a backpacking trip.
 */
struct TripRequirements {
    QString locationName = "Trek";
    QString description = "";
    
    double miles = 0.0;
    double elevationGain = 0.0;
    int durationHours = -1;
    
    // Resource overrides (if -1, they will be calculated based on miles/gain/hiker)
    int customCalories = -1;
    double customHydration = -1;

    TerrainType terrain = TerrainType::ALL;
    WeatherType weather = WeatherType::DEFAULT;
    AdventureType adventure = AdventureType::ALL;

    QString scienceFocus = "";
    double passingScore = 70.0;
};

#endif // TRIPCONDITIONS_H
