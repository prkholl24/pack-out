/**
 * @file player.h
 * @brief Header file for the Player class.
 *
 * Defines the Player class, which represents the user in the game.
 * The Player manages a backpack of items and provides access to
 * total calories, water, and weight based on its contents.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 */

#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
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
 * The Player class manages both the user's physical profile (used for 
 * calorie calculations) and their backpack of items.
 */
class Player
{
public:
    /**
     * @brief Constructs a new Player object with default physical stats.
     */
    Player();

    /**
     * @brief Destroys the Player object and clears the backpack.
     */
    ~Player();

    // --- Physical Profile ---
    double weightLbs = 160.0;
    double heightInches = 68.0;
    int age = 25;
    Gender gender = Gender::MALE;

    /**
     * @brief Calculates the daily Basal Metabolic Rate (BMR).
     *
     * @return Estimated daily calories burned at rest.
     */
    double calculateDailyBMR() const;

    // --- Backpack Management ---

    /**
     * @brief Adds an item to the player's backpack.
...
     *
     * The Player takes ownership of the item pointer.
     *
     * @param item The item to add.
     */
    void addItem(Item* item);

    /**
     * @brief Removes a specific item from the backpack and deletes it.
     *
     * @param item The item pointer to remove.
     */
    void removeItem(Item* item);

    /**
     * @brief Clears all items from the backpack and deletes them.
     */
    void clearBackpack();

    /**
     * @brief Gets the current items in the backpack.
     *
     * @return A const reference to the vector of item pointers.
     */
    const std::vector<Item*>& getBackpack() const;

    /**
     * @brief Calculates the total weight of all items in the backpack.
     *
     * @return The total weight in pounds.
     */
    double getTotalWeight() const;

private:
    /**
     * @brief The collection of items currently packed.
     */
    std::vector<Item*> backpack;
};

#endif // PLAYER_H
