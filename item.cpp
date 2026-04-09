/**
 * @file item.cpp
 * @brief Implementation of the Item class.
 *
 * Implements the base Item functionality, including shared behavior
 * for all items, such as weight access, tooltip generation,
 * and default stat handling.
 *
 * The Item class provides shared functionality used by all item types
 * and acts as the base for how items are represented in the backpacking system.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 */

#include "item.h"

Item::Item() : weight(0.0), description(nullptr)
{
}

double Item::getWeight() const
{
    return this->weight;
}

QString Item::getToolTip() const
{
    std::map<QString, double> stats = getStats();
    QString toolTipText;

    for (auto const& [name, value] : stats) {
        toolTipText += name + ": " + QString::number(value) + "\n";
    }

    if (description != nullptr) {
        toolTipText += "\n" + *description;
    }

    return toolTipText;
}

std::map<QString, double> Item::getStats() const
{
    std::map<QString, double> stats;
    stats["Weight(lbs)"] = weight;
    return stats;
}
