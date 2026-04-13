#include "itemregistry.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
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
    stats.shape = parseShape(obj.value("shape").toObject());
    return stats;
}

bool ItemRegistry::exists(const QString& name) {
    ensureLoaded();
    return itemsRoot.value("food").toObject().contains(name) ||
           itemsRoot.value("drink").toObject().contains(name) ||
           itemsRoot.value("gear").toObject().contains(name);
}

QString ItemRegistry::categoryOf(const QString& name) {
    ensureLoaded();
    if (itemsRoot.value("food").toObject().contains(name))  return QStringLiteral("food");
    if (itemsRoot.value("drink").toObject().contains(name)) return QStringLiteral("drink");
    if (itemsRoot.value("gear").toObject().contains(name))  return QStringLiteral("gear");
    return QString();
}

QStringList ItemRegistry::getFoodNames() {
    ensureLoaded();
    return itemsRoot.value("food").toObject().keys();
}

QStringList ItemRegistry::getDrinkNames() {
    ensureLoaded();
    return itemsRoot.value("drink").toObject().keys();
}

QStringList ItemRegistry::getGearNames() {
    ensureLoaded();
    return itemsRoot.value("gear").toObject().keys();
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

    stats.shape = parseShape(obj.value("shape").toObject());

    return stats;
}

ItemShape ItemRegistry::parseShape(const QJsonObject& obj) {
    ItemShape shape;
    if (obj.isEmpty()) return shape;

    shape.color = QColor(obj.value("color").toString("#888888"));
    shape.density = static_cast<float>(obj.value("density").toDouble(1.0));

    QJsonArray fixturesArr = obj.value("fixtures").toArray();
    for (const QJsonValue& fVal : fixturesArr) {
        QJsonObject fObj = fVal.toObject();
        ItemFixture fix;

        if (fObj.contains("box")) {
            fix.isBox = true;
            QJsonArray boxArr = fObj.value("box").toArray();
            fix.halfW = static_cast<float>(boxArr[0].toDouble());
            fix.halfH = static_cast<float>(boxArr[1].toDouble());
            if (fObj.contains("center")) {
                QJsonArray cArr = fObj.value("center").toArray();
                fix.centerX = static_cast<float>(cArr[0].toDouble());
                fix.centerY = static_cast<float>(cArr[1].toDouble());
            }
        } else if (fObj.contains("vertices")) {
            QJsonArray vertsArr = fObj.value("vertices").toArray();
            for (const QJsonValue& vVal : vertsArr) {
                QJsonArray pt = vVal.toArray();
                fix.vertices.emplace_back(
                    static_cast<float>(pt[0].toDouble()),
                    static_cast<float>(pt[1].toDouble())
                );
            }
        }

        shape.fixtures.push_back(std::move(fix));
    }

    return shape;
}
