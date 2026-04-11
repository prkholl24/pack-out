/**
 * @file level.cpp
 * @brief Implementation of the Level class
 */

#include "drinkitem.h"
#include "gearitem.h"
#include "level.h"

const double DEFAULT_PASSING_SCORE = 70.0;

const double ENERGY_SCORE_WEIGHT = 50.0;
const double HYDRATION_SCORE_WEIGHT = 50.0;
const double IDEAL_WEIGHT_PER_HOUR = 2.0;
const double CAFFEINE_HYDRATION_PENALTY_PER_MG = 0.02;
const double CAFFEINE_ENERGY_BONUS_PER_MG = 0.05;
const double ALCOHOL_HYDRATION_PENALTY_PER_UNIT = 1.5;
const double SUGAR_ENERGY_PENALTY_THRESHOLD = 50.0;
const double SUGAR_ENERGY_PENALTY_FACTOR = 0.5;
const double CAFFEINE_WARNING_THRESHOLD = 150.0;
const double SUGAR_WARNING_THRESHOLD = 50.0;

const double CALORIES_PER_MILE_PER_LB = 0.035;
const double CALORIES_PER_VERTICAL_FOOT_LB = 0.005;
const double HYDRATION_OZ_PER_HOUR = 8.0;

/**
 * @brief Represents environmental factors for a specific terrain type.
 */
struct TerrainProfile {
    double calorieMultiplier;
    double hydrationMultiplier;
};

/**
 * @brief Represents environmental factors for specific weather.
 */
struct WeatherProfile {
    double calorieMultiplier;
    double hydrationMultiplier;
};

static const std::map<TerrainType, TerrainProfile> TerrainRegistry = {
    { TerrainType::DESERT,   { 1.2, 2.0 } },
    { TerrainType::MOUNTAIN, { 1.3, 1.25 } },
    { TerrainType::ROCKY,    { 1.3, 1.25 } },
    { TerrainType::FOREST,   { 1.1, 1.05 } },
    { TerrainType::ALL,      { 1.0, 1.0 } }
};

static const std::map<WeatherType, WeatherProfile> WeatherRegistry = {
    { WeatherType::HOT,     { 1.0, 2.0 } },
    { WeatherType::COLD,    { 1.2, 0.8 } },
    { WeatherType::SNOW,    { 1.4, 0.9 } },
    { WeatherType::WINDY,   { 1.1, 1.1 } },
    { WeatherType::RAINY,   { 1.1, 1.2 } },
    { WeatherType::DEFAULT, { 1.0, 1.0 } }
};

const double HOURS_IN_DAY = 24.0;
const double PACK_WEIGHT_SCALER = 0.15;
const double ELEVATION_GAIN_STEP = 100.0;
const double HIGH_INTENSITY_ELEVATION_THRESHOLD = 1000.0;
const double HIGH_INTENSITY_HYDRATION_BONUS = 2.0;
const double SCORE_NORMALIZATION_FACTOR = 2.0;
const double PERFECT_SCORE = 100.0;
const double GEAR_MISSING_PENALTY = 0.85;

Level::Level()
    : miles(0.0), elevationGain(0.0), requiredCalories(-1), requiredHydration(-1), duration(-1),
    description(""), locationName("Trek"),
    requiredTerrain(TerrainType::ALL), requiredAdventure(AdventureType::ALL),
    requiredWeather(WeatherType::DEFAULT),
    passingScore(DEFAULT_PASSING_SCORE)
{
}

Level::Level(QString name, int calories, double water, int duration,
             TerrainType terrain, AdventureType adventure, WeatherType weather,
             double miles, double elevationGain)
    : miles(miles), elevationGain(elevationGain), requiredCalories(calories), requiredHydration(water), duration(duration),
    description(""), locationName(name),
    requiredTerrain(terrain), requiredAdventure(adventure),
    requiredWeather(weather),
    passingScore(DEFAULT_PASSING_SCORE)
{
}

QString Level::getLocationName() const { return locationName; }
int Level::getRequiredCalories() const { return requiredCalories; }
double Level::getRequiredHydration() const { return requiredHydration; }
int Level::getDuration() const { return duration; }
TerrainType Level::getTerrain() const { return requiredTerrain; }
AdventureType Level::getAdventure() const { return requiredAdventure; }
WeatherType Level::getWeather() const { return requiredWeather; }
QString Level::getDescription() const { return description; }

double Level::gradeMyBag(const std::vector<Item*>& backpack, const Player& player) const
{
    LevelResult result = evaluateBackpack(backpack, player);
    return result.grade;
}

bool Level::passesLevel(const std::vector<Item*>& backpack, const Player& player) const
{
    return gradeMyBag(backpack, player) >= passingScore;
}

double Level::calculateTotalCalories(const std::vector<Item*>& backpack) const
{
    double total = 0.0;
    for (const Item* i : backpack) if (i) total += i->getNutrition().calories;
    return total;
}

double Level::calculateTotalCaffeine(const std::vector<Item*>& backpack) const
{
    double total = 0.0;
    for (const Item* i : backpack) if (i) total += i->getNutrition().caffeineMg;
    return total;
}

double Level::calculateTotalAlcohol(const std::vector<Item*>& backpack) const
{
    double total = 0.0;
    for (const Item* i : backpack) if (i) total += i->getNutrition().alcohol;
    return total;
}

double Level::calculateTotalSugar(const std::vector<Item*>& backpack) const
{
    double total = 0.0;
    for (const Item* i : backpack) if (i) total += i->getNutrition().sugar;
    return total;
}

double Level::calculateTotalFiber(const std::vector<Item*>& backpack) const
{
    double total = 0.0;
    for (const Item* i : backpack) if (i) total += i->getNutrition().fiber;
    return total;
}

double Level::calculateTotalSodium(const std::vector<Item*>& backpack) const
{
    double total = 0.0;
    for (const Item* i : backpack) if (i) total += i->getNutrition().sodium;
    return total;
}

double Level::calculateTotalWeight(const std::vector<Item*>& backpack) const
{
    double total = 0.0;
    for (const Item* i : backpack) if (i) total += i->getWeight();
    return total;
}

double Level::calculateEffectiveHydration(const std::vector<Item*>& backpack) const
{
    double effective = 0.0;
    double totalCaffeine = 0.0;
    double totalAlcohol = 0.0;

    for (const Item* item : backpack) {
        if (!item) continue;
        const DrinkItem* drink = dynamic_cast<const DrinkItem*>(item);
        if (drink) effective += drink->getEffectiveHydration();
        const NutritionProfile& n = item->getNutrition();
        totalCaffeine += n.caffeineMg;
        totalAlcohol += n.alcohol;
    }

    effective -= totalCaffeine * CAFFEINE_HYDRATION_PENALTY_PER_MG;
    effective -= totalAlcohol * ALCOHOL_HYDRATION_PENALTY_PER_UNIT;
    return std::max(0.0, effective);
}

double Level::calculateEnergyScore(double totalCalories, double requiredCalories, double totalCaffeine,
                                double totalSugar) const
{
    if (requiredCalories <= 0) return 0.0;
    double score = (totalCalories / requiredCalories) * ENERGY_SCORE_WEIGHT;
    score += totalCaffeine * CAFFEINE_ENERGY_BONUS_PER_MG;
    if (totalSugar > SUGAR_ENERGY_PENALTY_THRESHOLD)
        score += (totalSugar - SUGAR_ENERGY_PENALTY_THRESHOLD) * SUGAR_ENERGY_PENALTY_FACTOR;
    return score;
}

double Level::calculateHydrationScore(double effectiveHydration, double requiredHydration) const
{
    if (requiredHydration <= 0) return 0.0;
    double score = (effectiveHydration / requiredHydration) * HYDRATION_SCORE_WEIGHT;
    return std::min(score, HYDRATION_SCORE_WEIGHT);
}

bool Level::hasRequiredGear(const std::vector<Item*>& backpack) const
{
    bool hasTerrainGear = (requiredTerrain == TerrainType::ALL);
    bool hasAdventureGear = (requiredAdventure == AdventureType::ALL);

    for (const Item* item : backpack) {
        const GearItem* gear = dynamic_cast<const GearItem*>(item);
        if (!gear) continue;
        if (gear->getTerrainType() == requiredTerrain) hasTerrainGear = true;
        if (gear->getAdventureType() == requiredAdventure) hasAdventureGear = true;
    }
    return hasTerrainGear && hasAdventureGear;
}

bool Level::hasWeatherGear(const std::vector<Item*>& backpack) const
{
    if (requiredWeather == WeatherType::DEFAULT) return true;
    for (const Item* item : backpack) {
        const GearItem* gear = dynamic_cast<const GearItem*>(item);
        if (!gear) continue;
        QString info = gear->getDescription().toLower();
        if (requiredWeather == WeatherType::SNOW && (info.contains("snow") || info.contains("winter") || info.contains("thermal"))) return true;
        if (requiredWeather == WeatherType::WINDY && (info.contains("wind") || info.contains("shell"))) return true;
        if (requiredWeather == WeatherType::HOT && (info.contains("sun") || info.contains("vented"))) return true;
        if (requiredWeather == WeatherType::COLD && (info.contains("jacket") || info.contains("wool"))) return true;
        if (requiredWeather == WeatherType::RAINY && (info.contains("rain") || info.contains("waterproof"))) return true;
    }
    return false;
}

LevelResult Level::evaluateBackpack(const std::vector<Item*>& backpack, const Player& player) const
{
    LevelResult result;

    result.totalCalories = calculateTotalCalories(backpack);
    result.effectiveHydration = calculateEffectiveHydration(backpack);
    result.totalCaffeine = calculateTotalCaffeine(backpack);
    result.totalSugar = calculateTotalSugar(backpack);
    result.totalAlcohol = calculateTotalAlcohol(backpack);
    result.totalWeight = calculateTotalWeight(backpack);

    auto tIt = TerrainRegistry.find(requiredTerrain);
    TerrainProfile tProfile = (tIt != TerrainRegistry.end()) ? tIt->second : TerrainProfile{1.0, 1.0};
    auto wIt = WeatherRegistry.find(requiredWeather);
    WeatherProfile wProfile = (wIt != WeatherRegistry.end()) ? wIt->second : WeatherProfile{1.0, 1.0};

    result.bmrCost = (player.calculateDailyBMR() / HOURS_IN_DAY) * duration;
    result.horizontalBurn = player.weightLbs * miles * CALORIES_PER_MILE_PER_LB * tProfile.calorieMultiplier * wProfile.calorieMultiplier;
    result.verticalBurn = player.weightLbs * elevationGain * CALORIES_PER_VERTICAL_FOOT_LB;
    result.packWeightPenalty = result.totalWeight * (miles + (elevationGain / ELEVATION_GAIN_STEP)) * PACK_WEIGHT_SCALER;

    double neededCals = result.bmrCost + result.horizontalBurn + result.verticalBurn + result.packWeightPenalty;
    result.requiredCalories = (requiredCalories > 0) ? requiredCalories : static_cast<int>(neededCals);
    
    double hRate = HYDRATION_OZ_PER_HOUR * tProfile.hydrationMultiplier * wProfile.hydrationMultiplier;
    if (elevationGain > HIGH_INTENSITY_ELEVATION_THRESHOLD) hRate += HIGH_INTENSITY_HYDRATION_BONUS;
    result.requiredHydration = (requiredHydration > 0) ? requiredHydration : (duration * hRate);

    double eScore = calculateEnergyScore(result.totalCalories, result.requiredCalories, result.totalCaffeine, result.totalSugar);
    double hScore = calculateHydrationScore(result.effectiveHydration, result.requiredHydration);

    bool needsEnergy = (result.requiredCalories > 0);
    bool needsHydration = (result.requiredHydration > 0);

    if (needsEnergy && needsHydration) result.grade = eScore + hScore;
    else if (needsEnergy) result.grade = eScore * SCORE_NORMALIZATION_FACTOR;
    else if (needsHydration) result.grade = hScore * SCORE_NORMALIZATION_FACTOR;
    else result.grade = PERFECT_SCORE;

    result.hasMissingGear = !hasRequiredGear(backpack) || !hasWeatherGear(backpack);
    if (result.hasMissingGear) result.grade *= GEAR_MISSING_PENALTY;
    result.passed = result.grade >= passingScore;

    result.idealWeight = duration * IDEAL_WEIGHT_PER_HOUR;
    result.isTooHeavy = (result.totalWeight > result.idealWeight && result.idealWeight > 0.0);
    result.isHighCaffeine = (result.totalCaffeine > CAFFEINE_WARNING_THRESHOLD);
    result.isHighSugar = (result.totalSugar > SUGAR_WARNING_THRESHOLD);
    result.isDehydrated = (needsHydration && result.effectiveHydration < result.requiredHydration);
    result.isStarving = (needsEnergy && result.totalCalories < result.requiredCalories);
    result.isDehydratedByAlcohol = (result.totalAlcohol > 0.0);

    double totalFiber = calculateTotalFiber(backpack);
    double totalSodium = calculateTotalSodium(backpack);
    bool hasCotton = false;
    for (const Item* i : backpack) {
        if (i && i->getDescription().toLower().contains("cotton")) { hasCotton = true; break; }
    }

    if (hasCotton && (requiredWeather == WeatherType::RAINY || requiredWeather == WeatherType::SNOW))
        result.lessonCode = ScienceLesson::COTTON_CLOTHING_WARNING;
    else if (totalSodium > 2300.0)
        result.lessonCode = ScienceLesson::SODIUM_HYDRATION_LOAD;
    else if (result.totalSugar > 50.0 && totalFiber < 10.0)
        result.lessonCode = ScienceLesson::FIBER_ENERGY_STABILITY;
    else if (result.isStarving && result.bmrCost > (result.requiredCalories * 0.4))
        result.lessonCode = ScienceLesson::BMR_BASE_METABOLISM;
    else if (result.isTooHeavy)
        result.lessonCode = ScienceLesson::PANDOLF_WEIGHT_PENALTY;
    else if (result.isHighCaffeine || result.isDehydratedByAlcohol)
        result.lessonCode = ScienceLesson::HYDRATION_DIURETICS;
    else
        result.lessonCode = ScienceLesson::PACK_WEIGHT_RATIO;

    return result;
}
