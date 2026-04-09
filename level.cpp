/**
 * @file level.cpp
 * @brief Implementation of the Level class
 *
 * Implements the Level model, including grading logic,
 * pass/fail evaluation, and feedback generation for
 * backpack preparedness.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 * Reviewed by Charles Adair
 */

#include "level.h"
#include "levelreportbuilder.h"

const double DEFAULT_PASSING_SCORE = 70.0;

const double ENERGY_SCORE_WEIGHT = 50.0;
const double HYDRATION_SCORE_WEIGHT = 50.0;

const double WATER_HYDRATION_MULTIPLIER = 1.0;
const double SPORTS_DRINK_HYDRATION_MULTIPLIER = 0.8;
const double SODA_HYDRATION_MULTIPLIER = 0.25;
const double CAFFEINE_HYDRATION_PENALTY_PER_MG = 0.02;
const double CAFFEINE_ENERGY_BONUS_PER_MG = 0.05;

Level::Level()
    : requiredCalories(-1), requiredHydration(-1), duration(-1), description(""),
    passingScore(DEFAULT_PASSING_SCORE)
{
}

Level::Level(int calories, double water, int duration)
    : requiredCalories(calories), requiredHydration(water), duration(duration), description(""),
    passingScore(DEFAULT_PASSING_SCORE)
{
}

int Level::getRequiredCalories() const
{
    return requiredCalories;
}

double Level::getRequiredHydration() const
{
    return requiredHydration;
}

int Level::getDuration() const
{
    return duration;
}

QString Level::getDescription() const
{
    return description;
}

double Level::gradeMyBag(const std::vector<Item*>& backpack) const
{
    double totalCalories = calculateTotalCalories(backpack);
    double effectiveHydration = calculateEffectiveHydration(backpack);
    double totalCaffeine = calculateTotalStat(backpack, "Caffeine(mg)");

    double energyScore = calculateEnergyScore(totalCalories, totalCaffeine);
    double hydrationScore = calculateHydrationScore(effectiveHydration);

    // Reduces energy score if hydration is too low.
    if (requiredHydration > 0)
    {
        double hydrationRatio = effectiveHydration / requiredHydration;

        if (hydrationRatio < 1.0)
            energyScore *= hydrationRatio;
    }

    return energyScore + hydrationScore;
}

bool Level::passesLevel(const std::vector<Item*>& backpack) const
{
    return gradeMyBag(backpack) >= passingScore;
}

QString Level::getReport(const std::vector<Item*>& backpack) const
{
    LevelResult result = evaluateBackpack(backpack);
    return LevelReportBuilder::buildReport(result);
}

// ---------- Helper Methods ----------
// --- Data collection ---

double Level::calculateTotalStat(const std::vector<Item*>& backpack, const QString& statName) const
{
    double total = 0.0;

    for (const Item* item : backpack)
    {
        if (item == nullptr)
            continue;

        std::map<QString, double> stats = item->getStats();

        if (stats.count(statName) > 0)
            total += stats.at(statName);
    }

    return total;
}

double Level::calculateTotalCalories(const std::vector<Item*>& backpack) const
{
    double totalCalories = 0.0;

    for (const Item* item : backpack)
    {
        if (item == nullptr)
            continue;

        std::map<QString, double> stats = item->getStats();

        if (stats.count("Calories") > 0)
            totalCalories += stats.at("Calories");
    }

    return totalCalories;
}

double Level::calculateEffectiveHydration(const std::vector<Item*>& backpack) const
{
    double totalWater = calculateTotalStat(backpack, "Water(oz)");
    double totalSportsDrink = calculateTotalStat(backpack, "SportsDrink(oz)");
    double totalSoda = calculateTotalStat(backpack, "Soda(oz)");
    double totalCaffeine = calculateTotalStat(backpack, "Caffeine(mg)");

    double effectiveHydration = 0.0;
    effectiveHydration += totalWater * WATER_HYDRATION_MULTIPLIER;
    effectiveHydration += totalSportsDrink * SPORTS_DRINK_HYDRATION_MULTIPLIER;
    effectiveHydration += totalSoda * SODA_HYDRATION_MULTIPLIER;
    effectiveHydration -= totalCaffeine * CAFFEINE_HYDRATION_PENALTY_PER_MG;

    if (effectiveHydration < 0.0)
        effectiveHydration = 0.0;

    return effectiveHydration;
}

// --- Scoring ---

double Level::calculateEnergyScore(double totalCalories, double totalCaffeine) const
{
    if (requiredCalories <= 0)
        return 0.0;

    double score = (totalCalories / requiredCalories) * ENERGY_SCORE_WEIGHT;

    // Add caffeine bonus
    score += totalCaffeine * CAFFEINE_ENERGY_BONUS_PER_MG;

    if (score > ENERGY_SCORE_WEIGHT)
        score = ENERGY_SCORE_WEIGHT;

    return score;
}

double Level::calculateHydrationScore(double effectiveHydration) const
{
    if (requiredHydration <= 0)
        return 0.0;

    double score = (effectiveHydration / requiredHydration) * HYDRATION_SCORE_WEIGHT;

    if (score > HYDRATION_SCORE_WEIGHT)
        score = HYDRATION_SCORE_WEIGHT;

    return score;
}

// --- Evaluation ---

LevelResult Level::evaluateBackpack(const std::vector<Item*>& backpack) const
{
    LevelResult result;

    result.totalCalories = calculateTotalCalories(backpack);
    result.effectiveHydration = calculateEffectiveHydration(backpack);
    result.grade = gradeMyBag(backpack);
    result.passed = result.grade >= passingScore;

    result.requiredCalories = requiredCalories;
    result.requiredHydration = requiredHydration;

    return result;
}
