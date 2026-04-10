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

Player::Player()
{
}

Player::~Player()
{
    clearBackpack();
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
