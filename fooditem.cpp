#include "fooditem.h"

FoodItem::FoodItem() : calories(0.0), sustenance(0.0) {}

FoodItem::FoodItem(QString& toolTip, double weight, QObject* parent, int calories, double sustenance)
     : PackableItem(toolTip, weight), calories(calories), sustenance(sustenance)
{}
