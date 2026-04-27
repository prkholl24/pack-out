/**
 * @file trekrules.h
 * @brief Global configuration manager for game rules and constants.
 */

#ifndef TREKRULES_H
#define TREKRULES_H

#include <QJsonObject>
#include <QStringList>
#include "tripconditions.h"

/**
 * @brief Grouped multipliers for environment effects.
 */
struct RuleModifier {
    double energy;
    double water;
};

/**
 * @brief Singleton-style manager that provides access to the rules of the game.
 */
class TrekRules
{
public:
    /**
     * @brief Accesses a specific numeric rule.
     */
    static double get(const QString& category, const QString& key);

    /**
     * @brief Gets multipliers for a specific terrain type.
     */
    static RuleModifier getTerrainModifier(TerrainType terrain);

    /**
     * @brief Gets multipliers for a specific weather type.
     */
    static RuleModifier getWeatherModifier(WeatherType weather);

    /**
     * @brief Gets the list of safety keywords for a weather type.
     */
    static QStringList getWeatherKeywords(WeatherType weather);

    /**
     * @brief Gets a tip string from data/feedback.json based on category and key.
     */
    static QString getTip(const QString& category, const QString& key);

    /**
     * @brief Generates a list of environmental tips for a level.
     */
    static QStringList getEnvironmentalTips(WeatherType weather, TerrainType terrain,
                                            AdventureType adventure, int durationHours,
                                            bool skipGearCheck, int customCalories);

    // --- Enum Conversion Helpers ---
    static QString terrainToString(TerrainType t);
    static QString weatherToString(WeatherType w);
    static QString adventureToString(AdventureType a);

private:
    static void ensureLoaded();
    static QJsonObject rulesRoot;
    static QJsonObject feedbackRoot;
};

#endif // TREKRULES_H
