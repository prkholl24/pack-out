/**
 * @file level.cpp
 * @brief Implementation of the Level orchestrator class.
 */

#include "level.h"
#include "drinkitem.h"
#include "trekphysiology.h"
#include "gearcheck.h"
#include "trekrules.h"
#include <algorithm>

Level::Level() : requirements(TripRequirements{}) {}
Level::Level(const TripRequirements& req) : requirements(req) {}

LevelResult Level::evaluateBackpack(const std::vector<std::unique_ptr<Item>>& backpack, const Player& player) const
{
    LevelResult result;
    
    // 1. Data Aggregation
    BackpackTotals totals = gatherBackpackTotals(backpack);
    mapTotalsToResult(result, totals, player);

    // 2. Physiology Math
    TrekPhysiology::computeRequirements(result, requirements, player, totals.hasPoles);

    // 3. Gear Safety Validation
    GearCheck::validate(backpack, requirements, result);

    // 4. Scoring logic
    applyEvaluationScoring(result, totals);
    
    // Physical state checks
    if (result.totalVolume > result.maxVolume) {
        result.conditions.push_back(TripCondition::TOO_BULKY);
    }
    if (result.totalWaste > 0 && !result.hasWasteStorage) {
        result.leftTrashBehind = true;
        result.conditions.push_back(TripCondition::LNT_VIOLATION);
    }

    applyEvaluationPenalties(result);

    // 5. Condition Generation
    generateStatusTags(result);
    result.lessonCode = determineScienceLesson(result);

    return result;
}

BackpackTotals Level::gatherBackpackTotals(const std::vector<std::unique_ptr<Item>>& backpack) const
{
    BackpackTotals totals;
    for (const auto& item : backpack) {
        if (!item) continue;
        
        totals.weightLbs += item->getWeight();
        totals.volume += item->getVolume();
        
        const NutritionProfile& n = item->getNutrition();
        totals.calories += n.calories;
        totals.sugar += n.sugar;
        totals.fiber += n.fiber;
        totals.sodium += n.sodium;
        totals.caffeine += n.caffeineMg;
        totals.alcohol += n.alcohol;
        
        const DrinkItem* drink = dynamic_cast<const DrinkItem*>(item.get());
        if (drink) {
            totals.hydrationOz += n.hydrationOz;
            totals.effectiveHydration += drink->getEffectiveHydration();
        }

        if (item->generatesWaste()) totals.wasteCount++;
        if (item->isWasteStorage()) totals.hasWasteStorage = true;
        if (item->isCottonMaterial()) totals.hasCotton = true;
        if (item->isTrekkingPole()) totals.hasPoles = true;
    }
    
    applyDiureticPenalties(totals);
    return totals;
}

void Level::mapTotalsToResult(LevelResult& result, const BackpackTotals& totals, const Player& player) const
{
    result.totalCalories = totals.calories;
    result.effectiveHydration = totals.effectiveHydration;
    result.totalCaffeine = totals.caffeine;
    result.totalSugar = totals.sugar;
    result.totalFiber = totals.fiber;
    result.totalSodium = totals.sodium;
    result.totalAlcohol = totals.alcohol;
    result.totalWeight = totals.weightLbs;
    result.totalVolume = totals.volume;
    result.totalWaste = totals.wasteCount;
    result.maxVolume = player.getMaxVolumeCapacity();
    result.hasWasteStorage = totals.hasWasteStorage;
    result.hasCottonWarning = totals.hasCotton;
}

void Level::applyDiureticPenalties(BackpackTotals& totals) const
{
    double caffeinePenalty = TrekRules::get("health_thresholds", "caffeine_hydration_penalty");
    double alcoholPenalty = TrekRules::get("health_thresholds", "alcohol_hydration_penalty");

    totals.effectiveHydration -= totals.caffeine * caffeinePenalty;
    totals.effectiveHydration -= totals.alcohol * alcoholPenalty;
    totals.effectiveHydration = std::max(0.0, totals.effectiveHydration);
}

void Level::applyEvaluationScoring(LevelResult& result, const BackpackTotals& totals) const
{
    double weightE = TrekRules::get("scoring", "energy_score_weight");
    double norm = TrekRules::get("scoring", "normalization_factor");
    double perfect = TrekRules::get("scoring", "perfect_score");

    double eScore = calculateEnergyScore(totals.calories, result.requiredCalories, totals.sugar);
    double hScore = calculateHydrationScore(totals.effectiveHydration, result.requiredHydration);

    if (result.requiredCalories > 0 && result.requiredHydration > 0) result.grade = eScore + hScore;
    else if (result.requiredCalories > 0) result.grade = eScore * norm;
    else if (result.requiredHydration > 0) result.grade = hScore * norm;
    else result.grade = perfect;
}

void Level::applyEvaluationPenalties(LevelResult& result) const
{
    if (result.hasMissingGear) result.grade *= TrekRules::get("penalties", "missing_gear");
    if (result.totalVolume > result.maxVolume) result.grade *= TrekRules::get("penalties", "bulky_pack");
    if (result.leftTrashBehind) result.grade *= TrekRules::get("penalties", "lnt_violation");
    if (result.missingEssentials) result.grade *= TrekRules::get("penalties", "missing_essentials");
    
    result.passed = result.grade >= requirements.passingScore;
}

void Level::generateStatusTags(LevelResult& result) const
{
    double idealWeightFactor = TrekRules::get("health_thresholds", "ideal_weight_lbs_per_hour");
    double caffeineThreshold = TrekRules::get("health_thresholds", "caffeine_warning_mg");
    double sugarThreshold = TrekRules::get("health_thresholds", "sugar_warning_grams");

    result.idealWeight = requirements.durationHours * idealWeightFactor;
    
    if (result.totalCalories < result.requiredCalories && result.requiredCalories > 0) 
        result.conditions.push_back(TripCondition::STARVING);
    if (result.effectiveHydration < result.requiredHydration && result.requiredHydration > 0) 
        result.conditions.push_back(TripCondition::DEHYDRATED);
    if (result.totalWeight > result.idealWeight && result.idealWeight > 0.0) 
        result.conditions.push_back(TripCondition::TOO_HEAVY);
    if (result.totalCaffeine > caffeineThreshold) 
        result.conditions.push_back(TripCondition::HIGH_CAFFEINE);
    if (result.totalSugar > sugarThreshold) 
        result.conditions.push_back(TripCondition::HIGH_SUGAR);
    if (result.totalAlcohol > 0.0) 
        result.conditions.push_back(TripCondition::ALCOHOL_PENALTY);
}

double Level::calculateEnergyScore(double calories, double required, double sugar) const
{
    if (required <= 0) return 0.0;
    
    double weight = TrekRules::get("scoring", "energy_score_weight");
    double threshold = TrekRules::get("health_thresholds", "sugar_penalty_threshold");
    double factor = TrekRules::get("penalties", "sugar_crash_factor");

    double score = (calories / required) * weight;
    if (sugar > threshold) score -= (sugar - threshold) * factor;
    return score;
}

double Level::calculateHydrationScore(double effective, double required) const
{
    if (required <= 0) return 0.0;
    double weight = TrekRules::get("scoring", "hydration_score_weight");
    double score = (effective / required) * weight;
    return std::min(score, weight);
}

ScienceLesson Level::determineScienceLesson(const LevelResult& result) const
{
    double bmrRatio = TrekRules::get("health_thresholds", "starving_bmr_ratio_threshold");

    if (result.leftTrashBehind) return ScienceLesson::LEAVE_NO_TRACE_VIOLATION;
    if (result.missingEssentials) return ScienceLesson::TEN_ESSENTIALS_MISSING;
    if (result.hasCottonWarning && (requirements.weather == WeatherType::RAINY || requirements.weather == WeatherType::SNOW))
        return ScienceLesson::COTTON_CLOTHING_WARNING;
    if (result.totalSodium > 2300.0) return ScienceLesson::SODIUM_HYDRATION_LOAD;
    if (result.totalSugar > 50.0 && result.totalFiber < 10.0) return ScienceLesson::FIBER_ENERGY_STABILITY;
    
    // Check for specific conditions in the conditions list
    bool isHighCaffeine = false;
    bool isAlcohol = false;
    bool isStarving = false;
    bool isTooBulky = false;
    bool isTooHeavy = false;

    for (TripCondition c : result.conditions) {
        if (c == TripCondition::HIGH_CAFFEINE) isHighCaffeine = true;
        if (c == TripCondition::ALCOHOL_PENALTY) isAlcohol = true;
        if (c == TripCondition::STARVING) isStarving = true;
        if (c == TripCondition::TOO_BULKY) isTooBulky = true;
        if (c == TripCondition::TOO_HEAVY) isTooHeavy = true;
    }

    if (isHighCaffeine || isAlcohol) return ScienceLesson::HYDRATION_DIURETICS;
    if (isStarving && result.bmrBurn > (result.requiredCalories * bmrRatio)) return ScienceLesson::BMR_BASE_METABOLISM;
    if (isTooBulky) return ScienceLesson::VOLUME_OVERFLOW;
    if (isTooHeavy) return ScienceLesson::PANDOLF_WEIGHT_PENALTY;
    
    return ScienceLesson::PACK_WEIGHT_RATIO;
}

double Level::gradeMyBag(const std::vector<std::unique_ptr<Item>>& backpack, const Player& player) const
{
    return evaluateBackpack(backpack, player).grade;
}

bool Level::passesLevel(const std::vector<std::unique_ptr<Item>>& backpack, const Player& player) const
{
    return evaluateBackpack(backpack, player).passed;
}
