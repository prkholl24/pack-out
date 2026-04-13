#ifndef ITEMREGISTRY_H
#define ITEMREGISTRY_H

#include <QColor>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <map>
#include <vector>
#include <utility>
#include "nutritionprofile.h"

/**
 * @brief Describes a single Box2D fixture for an item's physics shape.
 *
 * Either a convex polygon (vertices non-empty) or a box (isBox == true).
 * Uses plain floats so ItemRegistry stays independent of Box2D headers.
 */
struct ItemFixture {
    std::vector<std::pair<float, float>> vertices;
    float halfW = 0, halfH = 0;
    float centerX = 0, centerY = 0;
    bool isBox = false;
};

/**
 * @brief Visual and physics shape definition for an item body.
 */
struct ItemShape {
    QColor color;
    float density = 1.0f;
    std::vector<ItemFixture> fixtures;
};

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
    // Visual / physics shape
    ItemShape shape;
};

class ItemRegistry {
public:
    static void ensureLoaded();
    static ItemStats getFoodStats(const QString& name);
    static ItemStats getDrinkStats(const QString& name);
    static ItemStats getGearStats(const QString& name);
    static bool exists(const QString& name);

    /// Returns which top-level category an item belongs to ("food", "drink", "gear", or "").
    static QString categoryOf(const QString& name);

    /// Returns all item names in the given category.
    static QStringList getFoodNames();
    static QStringList getDrinkNames();
    static QStringList getGearNames();

private:
    static QJsonObject itemsRoot;
    static ItemStats parseStats(const QJsonObject& obj);
    static ItemShape parseShape(const QJsonObject& obj);
};

#endif // ITEMREGISTRY_H
