#ifndef FOODITEM_H
#define FOODITEM_H

#include "Item.h"

class FoodItem : public PackableItem
{
public:
    FoodItem();
    FoodItem(QString& toolTip, double weight, QObject* parent, int calories, double sustenance);

private:
    int calories;
    double sustenance;

signals:

private slots:

};

#endif // FOODITEM_H
