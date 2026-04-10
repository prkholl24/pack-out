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

#include "drinkitem.h"
#include "level.h"
#include "levelreportbuilder.h"

const double DEFAULT_PASSING_SCORE = 70.0;

const double ENERGY_SCORE_WEIGHT = 50.0;
const double HYDRATION_SCORE_WEIGHT = 50.0;
const double IDEAL_WEIGHT_PER_HOUR = 2.0;
const double CAFFEINE_HYDRATION_PENALTY_PER_MG = 0.02;
const double CAFFEINE_ENERGY_BONUS_PER_MG = 0.05;
const double ALCOHOL_HYDRATION_PENALTY_PER_UNIT = 1.5;
const double SUGAR_ENERGY_PENALTY_THRESHOLD = 50.0;
const double SUGAR_ENERGY_PENALTY_FACTOR = 0.5;

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
    double totalCaffeine = calculateTotalCaffeine(backpack);
    double totalSugar = calculateTotalSugar(backpack);

    double totalWeight = calculateTotalWeight(backpack);
    double energyScore = calculateEnergyScore(totalCalories, totalCaffeine, totalSugar, totalWeight);
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

double Level::calculateTotalCalories(const std::vector<Item*>& backpack) const
{
    double totalCalories = 0.0;

    for (const Item* item : backpack)
    {
        if (item == nullptr)
            continue;

        totalCalories += item->getNutrition().calories;
    }

    return totalCalories;
}

double Level::calculateTotalCaffeine(const std::vector<Item*>& backpack) const
{
    double totalCaffeine = 0.0;

    for (const Item* item : backpack)
    {
        if (item == nullptr)
            continue;

        totalCaffeine += item->getNutrition().caffeineMg;
    }

    return totalCaffeine;
}

double Level::calculateTotalAlcohol(const std::vector<Item*>& backpack) const
{
    double totalAlcohol = 0.0;

    for (const Item* item : backpack)
    {
        if (item == nullptr)
            continue;

        totalAlcohol += item->getNutrition().alcohol;
    }

    return totalAlcohol;
}

double Level::calculateEffectiveHydration(const std::vector<Item*>& backpack) const
{
    double effectiveHydration = 0.0;
    double totalCaffeine = 0.0;
    double totalAlcohol = 0.0;

    for (const Item* item : backpack)
    {
        if (item == nullptr)
            continue;

        const DrinkItem* drink = dynamic_cast<const DrinkItem*>(item);

        // Add hydration from drinks (already includes multiplier)
        if (drink != nullptr)
            effectiveHydration += drink->getEffectiveHydration();

        // Collect penalties from ALL items (food + drinks if applicable)
        const NutritionProfile& nutrition = item->getNutrition();
        totalCaffeine += nutrition.caffeineMg;
        totalAlcohol += nutrition.alcohol;
    }

    // Apply penalties
    effectiveHydration -= totalCaffeine * CAFFEINE_HYDRATION_PENALTY_PER_MG;
    effectiveHydration -= totalAlcohol * ALCOHOL_HYDRATION_PENALTY_PER_UNIT;

    if (effectiveHydration < 0.0)
        effectiveHydration = 0.0;

    return effectiveHydration;
}

// --- Scoring ---

double Level::calculateEnergyScore(double totalCalories, double totalCaffeine,
                                double totalSugar, double totalWeight) const
{
    if (requiredCalories <= 0)
        return 0.0;

    double score = (totalCalories / requiredCalories) * ENERGY_SCORE_WEIGHT;

    // Add caffeine bonus
    score += totalCaffeine * CAFFEINE_ENERGY_BONUS_PER_MG;

    // Add sugar bonus
    if (totalSugar > SUGAR_ENERGY_PENALTY_THRESHOLD)
    {
        score += (totalSugar - SUGAR_ENERGY_PENALTY_THRESHOLD) * SUGAR_ENERGY_PENALTY_FACTOR;
    }

    double idealWeight = duration * IDEAL_WEIGHT_PER_HOUR;

    if (idealWeight > 0.0)
    {
        double weightRatio = totalWeight / idealWeight;

        if (weightRatio > 1.0)
            score /= weightRatio;
    }

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

double Level::calculateTotalWeight(const std::vector<Item*>& backpack) const
{
    double totalWeight = 0.0;

    for (const Item* item : backpack)
    {
        if (item != nullptr)
        {
            totalWeight += item->getWeight();
        }
    }

    return totalWeight;
}

// --- Evaluation ---

LevelResult Level::evaluateBackpack(const std::vector<Item*>& backpack) const
{
    LevelResult result;

    result.totalCalories = calculateTotalCalories(backpack);
    result.effectiveHydration = calculateEffectiveHydration(backpack);
    result.totalCaffeine = calculateTotalCaffeine(backpack);
    result.totalSugar = calculateTotalSugar(backpack);
    result.totalAlcohol = calculateTotalAlcohol(backpack);

    result.grade = gradeMyBag(backpack);
    result.passed = result.grade >= passingScore;

    result.requiredCalories = requiredCalories;
    result.requiredHydration = requiredHydration;

    return result;
}

double Level::calculateTotalSugar(const std::vector<Item*>& backpack) const
{
    double totalSugar = 0.0;

    for (const Item* item : backpack)
    {
        if (item == nullptr)
            continue;

        totalSugar += item->getNutrition().sugar;
    }

    return totalSugar;
}
