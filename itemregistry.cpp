#include "itemregistry.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

QJsonObject ItemRegistry::itemsRoot;

void ItemRegistry::ensureLoaded() {
    if (!itemsRoot.isEmpty()) return;

    QFile file(":/data/items.json");
    if (file.open(QIODevice::ReadOnly)) {
        itemsRoot = QJsonDocument::fromJson(file.readAll()).object();
    } else {
        qWarning() << "CRITICAL: Could not load items.json!";
    }
}

ItemStats ItemRegistry::getFoodStats(const QString& name) {
    ensureLoaded();
    return parseStats(itemsRoot.value("food").toObject().value(name).toObject());
}

ItemStats ItemRegistry::getDrinkStats(const QString& name) {
    ensureLoaded();
    ItemStats stats = parseStats(itemsRoot.value("drink").toObject().value(name).toObject());
    stats.hydrationMultiplier = itemsRoot.value("drink").toObject().value(name).toObject().value("hydration_multiplier").toDouble(1.0);
    return stats;
}

ItemStats ItemRegistry::getGearStats(const QString& name) {
    ensureLoaded();
    QJsonObject obj = itemsRoot.value("gear").toObject().value(name).toObject();
    ItemStats stats;
    stats.weight = obj.value("weight").toDouble();
    stats.volume = obj.value("volume").toDouble();
    stats.description = obj.value("description").toString();
    stats.category = obj.value("category").toString();
    stats.isCotton = obj.value("is_cotton").toBool(false);
    stats.isWasteStorage = obj.value("is_waste_storage").toBool(false);
    stats.isPole = obj.value("is_pole").toBool(false);
    return stats;
}

bool ItemRegistry::exists(const QString& name) {
    ensureLoaded();
    return itemsRoot.value("food").toObject().contains(name) ||
           itemsRoot.value("drink").toObject().contains(name) ||
           itemsRoot.value("gear").toObject().contains(name);
}

ItemStats ItemRegistry::parseStats(const QJsonObject& obj) {
    ItemStats stats;
    stats.weight = obj.value("weight").toDouble();
    stats.volume = obj.value("volume").toDouble();
    stats.producesWaste = obj.value("produces_waste").toBool();
    stats.description = obj.value("description").toString();

    stats.nutrition.calories = obj.value("calories").toDouble();
    stats.nutrition.sugar = obj.value("sugar").toDouble();
    stats.nutrition.carbs = obj.value("carbs").toDouble();
    stats.nutrition.protein = obj.value("protein").toDouble();
    stats.nutrition.fat = obj.value("fat").toDouble();
    stats.nutrition.fiber = obj.value("fiber").toDouble();
    stats.nutrition.sodium = obj.value("sodium").toDouble();
    stats.nutrition.hydrationOz = obj.value("oz").toDouble();
    stats.nutrition.caffeineMg = obj.value("caffeine").toDouble();
    stats.nutrition.alcohol = obj.value("alcohol").toDouble();

    return stats;
}
