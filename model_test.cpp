/**
 * @file model_test.cpp
 * @brief Unit tests for the backpacking model and evaluation logic.
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>
#include "level.h"
#include "player.h"
#include "fooditem.h"
#include "drinkitem.h"
#include "gearitem.h"
#include "trekregistry.h"
#include "trekrules.h"

namespace {
bool approx(double actual, double expected, double epsilon = 0.01) {
    return std::abs(actual - expected) <= epsilon;
}
}

void testBMR() {
    std::cout << "Testing BMR Calculation... " << std::flush;
    Player p;
    p.setWeightLbs(160.0);
    p.setHeightInches(68.0);
    p.setAge(25);
    p.setGender(Gender::MALE);
    
    double bmr = p.calculateDailyBMR();
    // Mifflin-St Jeor: (10 * 72.57kg) + (6.25 * 172.72cm) - (5 * 25) + 5
    // 725.7 + 1079.5 - 125 + 5 = 1685.2
    assert(bmr > 1680 && bmr < 1690);
    std::cout << "PASSED" << std::endl;
}

void testBackpackAggregation() {
    std::cout << "Testing Backpack Aggregation... " << std::flush;
    Player p;
    p.addItem(std::make_unique<FoodItem>(Food::BANANA)); // 105 cals, 0.26 lbs
    p.addItem(std::make_unique<DrinkItem>(DrinkType::WATER_BOTTLE)); // 32 oz, 2.28 lbs
    
    assert(p.getTotalWeight() > 2.50 && p.getTotalWeight() < 2.60);
    assert(p.getBackpack().size() == 2);
    std::cout << "PASSED" << std::endl;
}

void testLevelScoring() {
    std::cout << "Testing Level Scoring (Angels Landing)... " << std::flush;
    TripRequirements req;
    req.locationName = "Test Angels";
    req.miles = 5.4;
    req.elevationGain = 1488.0;
    req.durationHours = 4;
    req.terrain = TerrainType::ROCKY;
    req.weather = WeatherType::HOT;
    
    Level level(req);
    Player p;
    
    // Evaluate empty backpack (should fail)
    LevelResult result = level.evaluateBackpack(p.getBackpack(), p);
    assert(result.passed == false);
    assert(result.grade < 50.0);
    
    std::cout << "PASSED" << std::endl;
}

void testGearMetadata() {
    std::cout << "Testing Gear Metadata Parsing... " << std::flush;

    GearItem poles("TREKKING_POLES");
    GearItem tent("TENT");
    GearItem wool("WOOL_SHIRT");

    assert(poles.matchesTerrain(TerrainType::MOUNTAIN));
    assert(tent.matchesAdventure(AdventureType::BACKPACKING));
    assert(wool.matchesWeatherKeywords(TrekRules::getWeatherKeywords(WeatherType::SNOW)));

    std::cout << "PASSED" << std::endl;
}

void testScoreCapAndOverpackPenalty() {
    std::cout << "Testing Score Cap and Overpack Penalties... " << std::flush;

    TripRequirements req;
    req.locationName = "Overpack Test";
    req.customCalories = 400;
    req.customHydration = 64.0;
    req.skipGearCheck = true;

    Level level(req);
    Player p;
    p.addItem(std::make_unique<FoodItem>("PEANUT_BUTTER_SANDWICH"));
    p.addItem(std::make_unique<DrinkItem>("WATER_CAMELBAK"));

    LevelResult noPenaltyWindow = level.evaluateBackpack(p.getBackpack(), p);
    assert(approx(noPenaltyWindow.grade, 100.0));

    req.customCalories = 336; // 420/336 = 1.25 -> 9.375 point calorie overpack deduction
    Level caloriePenaltyLevel(req);
    LevelResult caloriePenalty = caloriePenaltyLevel.evaluateBackpack(p.getBackpack(), p);
    assert(approx(caloriePenalty.grade, 90.625, 0.05));

    req.customCalories = 420;
    req.customHydration = 64.0 / 1.5; // 150% hydration overpack -> full 25 point deduction
    Level hydrationPenaltyLevel(req);
    LevelResult hydrationPenalty = hydrationPenaltyLevel.evaluateBackpack(p.getBackpack(), p);
    assert(approx(hydrationPenalty.grade, 75.0, 0.05));

    std::cout << "PASSED" << std::endl;
}

void testExplicitRequiredGear() {
    std::cout << "Testing Explicit Required Gear... " << std::flush;

    std::vector<Level> treks = TrekRegistry::loadTreks();
    assert(treks.size() >= 3);

    Player missingTent;
    missingTent.addItem(std::make_unique<GearItem>("COMPASS"));
    missingTent.addItem(std::make_unique<GearItem>("FLASHLIGHT"));
    missingTent.addItem(std::make_unique<GearItem>("SUNSCREEN"));
    missingTent.addItem(std::make_unique<GearItem>("FIRST_AID_KIT"));
    missingTent.addItem(std::make_unique<GearItem>("POCKET_KNIFE"));
    missingTent.addItem(std::make_unique<GearItem>("MATCHES"));
    missingTent.addItem(std::make_unique<GearItem>("WOOL_SHIRT"));
    missingTent.addItem(std::make_unique<GearItem>("SLEEPING_BAG"));
    missingTent.addItem(std::make_unique<GearItem>("TRASH_BAG"));

    LevelResult missingTentResult = treks[2].evaluateBackpack(missingTent.getBackpack(), missingTent);
    assert(missingTentResult.hasMissingGear);

    Player withTent;
    withTent.addItem(std::make_unique<GearItem>("COMPASS"));
    withTent.addItem(std::make_unique<GearItem>("FLASHLIGHT"));
    withTent.addItem(std::make_unique<GearItem>("SUNSCREEN"));
    withTent.addItem(std::make_unique<GearItem>("FIRST_AID_KIT"));
    withTent.addItem(std::make_unique<GearItem>("POCKET_KNIFE"));
    withTent.addItem(std::make_unique<GearItem>("MATCHES"));
    withTent.addItem(std::make_unique<GearItem>("WOOL_SHIRT"));
    withTent.addItem(std::make_unique<GearItem>("SLEEPING_BAG"));
    withTent.addItem(std::make_unique<GearItem>("TRASH_BAG"));
    withTent.addItem(std::make_unique<GearItem>("TENT"));

    LevelResult withTentResult = treks[2].evaluateBackpack(withTent.getBackpack(), withTent);
    assert(!withTentResult.hasMissingGear);

    std::cout << "PASSED" << std::endl;
}

void testSodiumLessonRequiresDehydration() {
    std::cout << "Testing Sodium Lesson Gating... " << std::flush;

    TripRequirements req;
    req.locationName = "Sodium Test";
    req.customCalories = 600;
    req.customHydration = 32.0;
    req.skipGearCheck = true;

    Level level(req);
    Player p;
    p.addItem(std::make_unique<FoodItem>("BEEF_JERKY"));
    p.addItem(std::make_unique<FoodItem>("BEEF_JERKY"));
    p.addItem(std::make_unique<FoodItem>("BEEF_JERKY"));
    p.addItem(std::make_unique<FoodItem>("BEEF_JERKY"));
    p.addItem(std::make_unique<DrinkItem>("WATER_BOTTLE"));

    LevelResult result = level.evaluateBackpack(p.getBackpack(), p);
    assert(result.totals.sodium > 2300.0);
    assert(result.totals.effectiveHydration >= result.requiredHydration);
    assert(result.lessonCode != ScienceLesson::SODIUM_HYDRATION_LOAD);

    req.customHydration = 64.0;
    Level dehydratedLevel(req);
    LevelResult dehydratedResult = dehydratedLevel.evaluateBackpack(p.getBackpack(), p);
    assert(dehydratedResult.totals.sodium > 2300.0);
    assert(dehydratedResult.totals.effectiveHydration < dehydratedResult.requiredHydration);
    assert(dehydratedResult.lessonCode == ScienceLesson::SODIUM_HYDRATION_LOAD);

    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "--- PACK OUT MODEL LOGIC TEST SUITE ---" << std::endl;
    
    try {
        testBMR();
        testBackpackAggregation();
        testLevelScoring();
        testGearMetadata();
        testScoreCapAndOverpackPenalty();
        testExplicitRequiredGear();
        testSodiumLessonRequiresDehydration();
        std::cout << "\nAll Model Logic Tests PASSED!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\nTest Suite Failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
