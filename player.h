/**
 * @file player.h
 * @brief Header file for the Player class.
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <memory>
#include <QObject>
#include <QString>
#include "item.h"

/**
 * @brief Enumerates biological gender for physiological calculations.
 */
enum class Gender {
    MALE,
    FEMALE
};

/**
 * @brief Represents the player, their physical stats, and their backpack.
 *
 * Inherits from QObject to support Signal/Slot communication. Manages 
 * ownership of items via unique_ptr and caches weight/volume for efficiency.
 */
class Player : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a Player with default stats.
     */
    explicit Player(QObject *parent = nullptr);
    
    ~Player();

    // --- Physical Profile ---
    double getWeightLbs() const;
    void setWeightLbs(double weight);

    double getHeightInches() const;
    void setHeightInches(double height);

    int getAge() const;
    void setAge(int age);

    Gender getGender() const;
    void setGender(Gender gender);

    /**
     * @brief Calculates the daily Basal Metabolic Rate (BMR) using Mifflin-St Jeor.
     * @return Calories burned per day at rest.
     */
    double calculateDailyBMR() const;

    // --- Backpack Management ---
    
    /**
     * @brief Adds an item and takes ownership.
     */
    void addItem(std::unique_ptr<Item> item);
    
    /**
     * @brief Removes an item by raw pointer comparison.
     */
    void removeItem(Item* item);
    
    /**
     * @brief Clears all items and resets ownership.
     */
    void clearBackpack();

    /**
     * @brief Returns a const reference to the backpack for evaluation.
     */
    const std::vector<std::unique_ptr<Item>>& getBackpack() const;

    // --- Accessors ---
    
    /**
     * @brief Returns the O(1) cached total weight.
     */
    double getTotalWeight() const;
    
    /**
     * @brief Returns the O(1) cached total volume.
     */
    double getTotalVolume() const;
    
    double getMaxVolumeCapacity() const;
    void setMaxVolumeCapacity(double capacity);

signals:
    /**
     * @brief Emitted whenever items are added or removed.
     */
    void backpackChanged();

    /**
     * @brief Emitted when any totals or stats change.
     */
    void statsChanged(double weight, double volume);

private:
    double weightLbs;
    double heightInches;
    int age;
    Gender gender;

    std::vector<std::unique_ptr<Item>> backpack;
    double maxVolumeCapacity;

    double cachedTotalWeight;
    double cachedTotalVolume;
};

#endif // PLAYER_H
