#ifndef ITEMREGISTRY_H
#define ITEMREGISTRY_H

#include <QJsonObject>
#include <QString>
#include <map>
#include "nutritionprofile.h"

struct ItemStats {
    double weight = 0.0;
    double volume = 0.0;
    bool producesWaste = false;
    QString description;
    NutritionProfile nutrition;
    // Drink specific
    double hydrationMultiplier = 1.0;
    // Gear specific
    QString category;
    bool isCotton = false;
    bool isWasteStorage = false;
    bool isPole = false;
};

class ItemRegistry {
public:
    static void ensureLoaded();
    static ItemStats getFoodStats(const QString& name);
    static ItemStats getDrinkStats(const QString& name);
    static ItemStats getGearStats(const QString& name);
    static bool exists(const QString& name);

private:
    static QJsonObject itemsRoot;
    static ItemStats parseStats(const QJsonObject& obj);
};

#endif // ITEMREGISTRY_H
