/**
 * @file model_test.cpp
 * @brief Unit tests for the backpacking model and evaluation engine.
 */

#include <iostream>
#include <cassert>
#include <memory>
#include "level.h"
#include "player.h"
#include "fooditem.h"
#include "drinkitem.h"
#include "gearitem.h"

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

int main() {
    std::cout << "--- PACK OUT MODEL LOGIC TEST SUITE ---" << std::endl;
    
    try {
        testBMR();
        testBackpackAggregation();
        testLevelScoring();
        std::cout << "\nAll Model Logic Tests PASSED!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\nTest Suite Failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
