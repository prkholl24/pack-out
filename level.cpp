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
    result.maxVolume = player.getMaxVolumeCapacity();
    result.hydrationOnly = (requirements.customCalories == 0);
    
    // 1. Data Aggregation
    result.totals = gatherBackpackTotals(backpack);

    // 2. Physiology Math
    TrekPhysiology::computeRequirements(result, requirements, player, result.totals.hasPoles);

    // 3. Gear Safety Validation
    GearCheck::validate(backpack, requirements, result);

    // 4. Scoring logic
    applyEvaluationScoring(result);
    
    // Physical state checks (Volume and LNT) only matter once gear is part of the game
    if (!requirements.skipGearCheck) {
        if (result.totals.volume > result.maxVolume) {
            result.conditions.push_back(TripCondition::TOO_BULKY);
        }
        if (result.totals.wasteCount > 0 && !result.totals.hasWasteStorage) {
            result.leftTrashBehind = true;
            result.conditions.push_back(TripCondition::LNT_VIOLATION);
        }
    }

    applyEvaluationPenalties(result);

    // 5. Condition Generation
    generateStatusConditions(result);
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

void Level::applyDiureticPenalties(BackpackTotals& totals) const
{
    double caffeinePenalty = TrekRules::get("health_thresholds", "caffeine_hydration_penalty");
    double alcoholPenalty = TrekRules::get("health_thresholds", "alcohol_hydration_penalty");

    totals.effectiveHydration -= totals.caffeine * caffeinePenalty;
    totals.effectiveHydration -= totals.alcohol * alcoholPenalty;
    totals.effectiveHydration = std::max(0.0, totals.effectiveHydration);
}

void Level::applyEvaluationScoring(LevelResult& result) const
{
    double norm = TrekRules::get("scoring", "normalization_factor");
    double perfect = TrekRules::get("scoring", "perfect_score");

    // If customCalories is 0, we are in a hydration-only level.
    // We ignore calorie scoring entirely.
    if (requirements.customCalories == 0) {
        double hScore = calculateHydrationScore(result.totals.effectiveHydration, result.requiredHydration);
        result.grade = hScore * norm;
    } else {
        double eScore = calculateEnergyScore(result.totals.calories, result.requiredCalories, result.totals.sugar);
        double hScore = calculateHydrationScore(result.totals.effectiveHydration, result.requiredHydration);

        if (result.requiredCalories > 0 && result.requiredHydration > 0) result.grade = eScore + hScore;
        else if (result.requiredCalories > 0) result.grade = eScore * norm;
        else if (result.requiredHydration > 0) result.grade = hScore * norm;
        else result.grade = perfect;
    }
}

void Level::applyEvaluationPenalties(LevelResult& result) const
{
    if (result.hasMissingGear) result.grade *= TrekRules::get("penalties", "missing_gear");
    
    bool isBulky = false;
    for (TripCondition c : result.conditions) if (c == TripCondition::TOO_BULKY) isBulky = true;
    if (isBulky) result.grade *= TrekRules::get("penalties", "bulky_pack");

    if (result.leftTrashBehind) result.grade *= TrekRules::get("penalties", "lnt_violation");
    if (result.missingEssentials) result.grade *= TrekRules::get("penalties", "missing_essentials");
    
    result.passed = result.grade >= requirements.passingScore;
}

void Level::generateStatusConditions(LevelResult& result) const
{
    double idealWeightFactor = TrekRules::get("health_thresholds", "ideal_weight_lbs_per_hour");
    double caffeineThreshold = TrekRules::get("health_thresholds", "caffeine_warning_mg");
    double sugarThreshold = TrekRules::get("health_thresholds", "sugar_warning_grams");

    result.idealWeight = requirements.durationHours * idealWeightFactor;
    
    // Only flag starving if calories are required (customCalories != 0)
    if (result.totals.calories < result.requiredCalories && result.requiredCalories > 0 && requirements.customCalories != 0) 
        result.conditions.push_back(TripCondition::STARVING);

    if (result.totals.effectiveHydration < result.requiredHydration && result.requiredHydration > 0) 
        result.conditions.push_back(TripCondition::DEHYDRATED);

    // Skip heavy pack warning in early/specialized levels where gear isn't weighed yet
    if (result.totals.weightLbs > result.idealWeight && result.idealWeight > 0.0 && requirements.customCalories != 0) 
        result.conditions.push_back(TripCondition::TOO_HEAVY);

    if (result.totals.caffeine > caffeineThreshold) 
        result.conditions.push_back(TripCondition::HIGH_CAFFEINE);
    if (result.totals.sugar > sugarThreshold) 
        result.conditions.push_back(TripCondition::HIGH_SUGAR);
    if (result.totals.alcohol > 0.0) 
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
    // --- Phase 1: The "True" Science Lesson ---
    // We calculate the most scientifically relevant lesson based on the actual backpack.
    // This phase uses YOUR original logic to find the biggest problem.
    ScienceLesson trueLesson = ScienceLesson::PACK_WEIGHT_RATIO;

    bool isHighCaffeine = false;
    bool isAlcohol = false;
    bool isStarving = false;
    bool isTooBulky = false;
    bool isTooHeavy = false;
    bool isDehydrated = false;

    for (TripCondition c : result.conditions) {
        if (c == TripCondition::HIGH_CAFFEINE) isHighCaffeine = true;
        if (c == TripCondition::ALCOHOL_PENALTY) isAlcohol = true;
        if (c == TripCondition::STARVING) isStarving = true;
        if (c == TripCondition::TOO_BULKY) isTooBulky = true;
        if (c == TripCondition::TOO_HEAVY) isTooHeavy = true;
        if (c == TripCondition::DEHYDRATED) isDehydrated = true;
    }

    // Priority hierarchy for the "True" lesson (most critical first)
    if (result.leftTrashBehind) trueLesson = ScienceLesson::LEAVE_NO_TRACE_VIOLATION;
    else if (result.missingEssentials) trueLesson = ScienceLesson::TEN_ESSENTIALS_MISSING;
    else if (result.totals.hasCotton && (requirements.weather == WeatherType::RAINY || requirements.weather == WeatherType::SNOW))
        trueLesson = ScienceLesson::COTTON_CLOTHING_WARNING;
    
    // HYDRATION SCIENCE: Prioritize interference (diuretics) over basic dehydration
    else if (isHighCaffeine || isAlcohol) trueLesson = ScienceLesson::HYDRATION_DIURETICS;
    else if (isDehydrated) trueLesson = ScienceLesson::HYDRATION_BASICS;
    
    else if (result.totals.sodium > 2300.0) trueLesson = ScienceLesson::SODIUM_HYDRATION_LOAD;
    else if (result.totals.sugar > 50.0 && result.totals.fiber < 10.0) trueLesson = ScienceLesson::FIBER_ENERGY_STABILITY;
    else {
        double bmrRatio = TrekRules::get("health_thresholds", "starving_bmr_ratio_threshold");
        if (isStarving && result.bmrBurn > (result.requiredCalories * bmrRatio)) trueLesson = ScienceLesson::BMR_BASE_METABOLISM;
        else if (isStarving) trueLesson = ScienceLesson::WHOLE_FOOD_FUEL;
        else if (isTooBulky) trueLesson = ScienceLesson::VOLUME_OVERFLOW;
        else if (isTooHeavy) trueLesson = ScienceLesson::PANDOLF_WEIGHT_PENALTY;
    }

    // --- Phase 2: The Level Filter (Hydration Focus) ---
    // If the level is set to "Hydration-Only" (customCalories == 0), we ensure 
    // the lesson is hydration-focused.
    bool hydrationOnly = (requirements.customCalories == 0);
    bool gearScored = !requirements.skipGearCheck;

    if (hydrationOnly) {
        // If your logic found a diuretic issue, show it! 
        // Otherwise, stick to Hydration Fundamentals.
        if (trueLesson == ScienceLesson::HYDRATION_DIURETICS) return trueLesson;
        return ScienceLesson::HYDRATION_BASICS;
    }

    if (!gearScored) {
        // Levels that don't score gear yet: mask gear-specific lessons.
        if (trueLesson == ScienceLesson::LEAVE_NO_TRACE_VIOLATION || 
            trueLesson == ScienceLesson::TEN_ESSENTIALS_MISSING ||
            trueLesson == ScienceLesson::COTTON_CLOTHING_WARNING ||
            trueLesson == ScienceLesson::VOLUME_OVERFLOW ||
            trueLesson == ScienceLesson::PANDOLF_WEIGHT_PENALTY ||
            trueLesson == ScienceLesson::PACK_WEIGHT_RATIO) {
            
            // If they failed because of food/water, show that.
            if (isStarving) return ScienceLesson::WHOLE_FOOD_FUEL;
            if (isDehydrated || isHighCaffeine || isAlcohol) return trueLesson;
            return ScienceLesson::HYDRATION_BASICS;
        }
    }

    // Advanced levels: show the full "True" science lesson.
    return trueLesson;
}

double Level::gradeMyBag(const std::vector<std::unique_ptr<Item>>& backpack, const Player& player) const
{
    return evaluateBackpack(backpack, player).grade;
}

bool Level::passesLevel(const std::vector<std::unique_ptr<Item>>& backpack, const Player& player) const
{
    return evaluateBackpack(backpack, player).passed;
}

void Level::setDescription(const QString& newDescription)
{
    requirements.description = newDescription;
}
