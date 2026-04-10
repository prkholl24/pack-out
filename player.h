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
#include "item.h"

/**
 * @brief Represents the player and their current state.
 *
 * The Player class owns the backpack and is responsible for
 * adding, removing, and managing the memory of the items packed.
 */
class Player
{
public:
    /**
     * @brief Constructs a new Player object.
     */
    Player();

    /**
     * @brief Destroys the Player object and clears the backpack.
     */
    ~Player();

    /**
     * @brief Adds an item to the player's backpack.
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
