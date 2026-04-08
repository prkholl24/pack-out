#include "Item.h"

/**
 * @brief PackableItem Default constructor.
 */
PackableItem::PackableItem() : weight(0.0), description(nullptr) {}

double PackableItem::getWeight() const {
    return this->weight;
}

QString PackableItem::getToolTip() {
    std::map<QString, double> stats = getStats();
    QString toolTipText = "";

    for (auto const& [name, value] : stats) {
        toolTipText += name + ": " + QString::number(value) + "\n";
    }

    if (description != nullptr) {
        toolTipText += "\n" + *description;
    }

    return toolTipText;
}

std::map<QString, double> PackableItem::getStats() {
    std::map<QString, double> stats;
    stats["Weight"] = weight;
    return stats;
}
