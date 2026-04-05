#ifndef FOODITEM_H
#define FOODITEM_H

#include "Item.h"

class FoodItem : public PackableItem
{
public:
    FoodItem();
    FoodItem(double calories, double sustenance, double weight);

private:
    double calories;
    double sustenance;

signals:

private slots:

};

#endif // FOODITEM_H
