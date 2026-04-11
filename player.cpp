/**
 * @file player.cpp
 * @brief Implementation of the Player class.
 *
 * Implements the backpack management logic, including adding,
 * removing, and calculating total weight of packed items.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#include "player.h"
#include <algorithm>

const double LBS_TO_KG = 0.453592;
const double INCHES_TO_CM = 2.54;

/**
 * @brief Groups the constants used in the Mifflin-St Jeor BMR equation.
 */
struct BMRCoefficients {
    double weightMult;
    double heightMult;
    double ageMult;
    double offset;
};

const BMRCoefficients MALE_BMR   = { 10.0, 6.25, 5.0, 5.0 };
const BMRCoefficients FEMALE_BMR = { 10.0, 6.25, 5.0, -161.0 };

Player::Player()
{
}

Player::~Player()
{
    clearBackpack();
}

double Player::calculateDailyBMR() const
{
    double weightKg = weightLbs * LBS_TO_KG;
    double heightCm = heightInches * INCHES_TO_CM;

    // Select the appropriate coefficients based on gender
    const BMRCoefficients& coeffs = (gender == Gender::MALE) ? MALE_BMR : FEMALE_BMR;

    return (coeffs.weightMult * weightKg) + 
           (coeffs.heightMult * heightCm) - 
           (coeffs.ageMult * age) + 
           coeffs.offset;
}

void Player::addItem(Item* item)
{
    if (item != nullptr)
    {
        backpack.push_back(item);
    }
}

void Player::removeItem(Item* item)
{
    auto it = std::find(backpack.begin(), backpack.end(), item);
    if (it != backpack.end())
    {
        delete *it;
        backpack.erase(it);
    }
}

void Player::clearBackpack()
{
    for (Item* item : backpack)
    {
        delete item;
    }
    backpack.clear();
}

const std::vector<Item*>& Player::getBackpack() const
{
    return backpack;
}

double Player::getTotalWeight() const
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
