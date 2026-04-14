/**
 * @file spritecache.cpp
 * @brief Implementation of the sprite cache and template generator.
 *
 * @authors Charles Adair, Claude Opus 4.6 (1M context)
 * @date 4/13/2026
 */
#include "spritecache.h"
#include <QDebug>

std::map<QString, SpriteEntry> SpriteCache::cache_;

// ---------------------------------------------------------------------------
//  Public API
// ---------------------------------------------------------------------------

void SpriteCache::init()
{
    ItemRegistry::ensureLoaded();

    // Collect every item name across all categories.
    QStringList allNames;
    allNames << ItemRegistry::getFoodNames()
             << ItemRegistry::getDrinkNames()
             << ItemRegistry::getGearNames();

    for (const QString& name : allNames) {
        // Load from Qt resource path  (e.g. ":/sprites/BANANA.png").
        QString resPath = QStringLiteral(":/sprites/") + name + QStringLiteral(".png");

        // Retrieve shape data for AABB computation.
        QString cat = ItemRegistry::categoryOf(name);
        ItemStats stats;
        if      (cat == "food")  stats = ItemRegistry::getFoodStats(name);
        else if (cat == "drink") stats = ItemRegistry::getDrinkStats(name);
        else if (cat == "gear")  stats = ItemRegistry::getGearStats(name);

        SpriteEntry entry;
        entry.aabb   = ItemRegistry::computeAABB(stats.shape);
        entry.pixmap = QPixmap(resPath);
        entry.valid  = !entry.pixmap.isNull();

        if (!entry.valid) {
            qWarning() << "SpriteCache: no sprite resource for" << name;
        }

        cache_[name] = std::move(entry);
    }

    qInfo() << "SpriteCache: loaded" << cache_.size() << "sprites";
}

const SpriteEntry& SpriteCache::get(const QString& itemName)
{
    auto it = cache_.find(itemName);
    if (it != cache_.end()) return it->second;
    // Deferred construction avoids QPixmap before QApplication exists.
    static SpriteEntry invalidEntry;
    return invalidEntry;
}

bool SpriteCache::has(const QString& itemName)
{
    auto it = cache_.find(itemName);
    return it != cache_.end() && it->second.valid;
}

