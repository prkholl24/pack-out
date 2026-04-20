/**
 * @file player.cpp
 * @brief Implementation of the Player class.
 */

#include "player.h"
#include <algorithm>

const double LBS_TO_KG = 0.453592;
const double INCHES_TO_CM = 2.54;

struct BMRCoefficients {
    double weightMult;
    double heightMult;
    double ageMult;
    double offset;
};

const BMRCoefficients MALE_BMR   = { 10.0, 6.25, 5.0, 5.0 };
const BMRCoefficients FEMALE_BMR = { 10.0, 6.25, 5.0, -161.0 };

Player::Player(QObject *parent)
    : QObject(parent), weightLbs(160.0), heightInches(68.0), age(25), gender(Gender::MALE),
      maxVolumeCapacity(20.0), cachedTotalWeight(0.0), cachedTotalVolume(0.0)
{
}

Player::~Player()
{
}

// --- Physical Profile ---

double Player::getWeightLbs() const { return weightLbs; }
void Player::setWeightLbs(double weight) { 
    if (weight > 0) {
        weightLbs = weight; 
        emit statsChanged(cachedTotalWeight, cachedTotalVolume);
    }
}

double Player::getHeightInches() const { return heightInches; }
void Player::setHeightInches(double height) { 
    if (height > 0) {
        heightInches = height; 
        emit statsChanged(cachedTotalWeight, cachedTotalVolume);
    }
}

int Player::getAge() const { return age; }
void Player::setAge(int a) { 
    if (a > 0 && a < 120) {
        age = a; 
        emit statsChanged(cachedTotalWeight, cachedTotalVolume);
    }
}

Gender Player::getGender() const { return gender; }
void Player::setGender(Gender g) { 
    gender = g; 
    emit statsChanged(cachedTotalWeight, cachedTotalVolume);
}

double Player::calculateDailyBMR() const
{
    double weightKg = weightLbs * LBS_TO_KG;
    double heightCm = heightInches * INCHES_TO_CM;
    const BMRCoefficients& coeffs = (gender == Gender::MALE) ? MALE_BMR : FEMALE_BMR;

    return (coeffs.weightMult * weightKg) + 
           (coeffs.heightMult * heightCm) - 
           (coeffs.ageMult * age) + 
           coeffs.offset;
}

// --- Backpack Management (Signals) ---

void Player::addItem(std::unique_ptr<Item> item)
{
    if (item)
    {
        cachedTotalWeight += item->getWeight();
        cachedTotalVolume += item->getVolume();
        backpack.push_back(std::move(item));
        
        emit backpackChanged();
        emit statsChanged(cachedTotalWeight, cachedTotalVolume);
    }
}

void Player::removeItem(Item* item)
{
    auto it = std::find_if(backpack.begin(), backpack.end(), 
        [item](const std::unique_ptr<Item>& ptr) { return ptr.get() == item; });
    
    if (it != backpack.end())
    {
        cachedTotalWeight -= (*it)->getWeight();
        cachedTotalVolume -= (*it)->getVolume();
        backpack.erase(it);
        
        emit backpackChanged();
        emit statsChanged(cachedTotalWeight, cachedTotalVolume);
    }
}

void Player::clearBackpack()
{
    backpack.clear();
    cachedTotalWeight = 0.0;
    cachedTotalVolume = 0.0;
    
    emit backpackChanged();
    emit statsChanged(cachedTotalWeight, cachedTotalVolume);
}

const std::vector<std::unique_ptr<Item>>& Player::getBackpack() const
{
    return backpack;
}

// --- Accessors ---

double Player::getTotalWeight() const { return cachedTotalWeight; }
double Player::getTotalVolume() const { return cachedTotalVolume; }
double Player::getMaxVolumeCapacity() const { return maxVolumeCapacity; }
void Player::setMaxVolumeCapacity(double capacity) { if (capacity > 0) maxVolumeCapacity = capacity; }

// --- Survival Guide progression ---

void Player::unlockLesson(ScienceLesson lesson)
{
    if (lesson == ScienceLesson::NONE) return;
    auto inserted = unlockedLessons.insert(lesson);
    if (inserted.second) emit lessonsChanged();
}

const std::set<ScienceLesson>& Player::getUnlockedLessons() const
{
    return unlockedLessons;
}

bool Player::hasLesson(ScienceLesson lesson) const
{
    return unlockedLessons.count(lesson) > 0;
}

void Player::setUnlockedLessons(std::set<ScienceLesson> lessons)
{
    lessons.erase(ScienceLesson::NONE);
    unlockedLessons = std::move(lessons);
    emit lessonsChanged();
}
