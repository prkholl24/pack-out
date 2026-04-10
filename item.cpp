/**
 * @file item.cpp
 * @brief Implementation of the Item base class.
 *
 * Implements shared functionality used by all packable items,
 * including weight access, nutrition access, tooltip generation,
 * and default stat reporting.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */

#include "item.h"

Item::Item()
{
    weight = 0.0;
    description = "";
    nutrition = NutritionProfile{};
}

double Item::getWeight() const
{
    return weight;
}

const NutritionProfile& Item::getNutrition() const
{
    return nutrition;
}

QString Item::getToolTip() const
{
    QString toolTipText;
    std::map<QString, double> stats = getStats();

    for (const auto& [name, value] : stats)
    {
        toolTipText += name + ": " + QString::number(value) + "\n";
    }

    if (!description.isEmpty())
    {
        toolTipText += "\n" + description;
    }

    return toolTipText;
}

std::map<QString, double> Item::getStats() const
{
    std::map<QString, double> stats;
    stats["Weight(lbs)"] = weight;
    return stats;
}
