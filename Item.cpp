#include "Item.h"

/**
 * @brief PackableItem Default constructor.
 * Creates an item with no toolTip and 0 weight.
 */
PackableItem::PackableItem() : toolTip(""), weight(0.0), parent(nullptr) {}

PackableItem::PackableItem(QString& toolTip, double weight, QObject* parent) : toolTip(toolTip), weight(weight), parent(parent)
{
    // Any fancy code that needs to happen goes here!
}

double PackableItem::getWeight() const {
    return this->weight;
}

void PackableItem::setWeight(double newWeight) {
    this->weight = newWeight;
    emit weightChanged(newWeight);
}
