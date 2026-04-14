/**
 * @file spritecache.h
 * @brief Loads and caches sprite images for items, auto-generating
 *        template PNGs for any item that lacks one.
 *
 * On init(), the cache scans items.json for every item name.  For each
 * item it checks whether a matching PNG exists on disk (in the sprites
 * directory).  If the file is missing, a transparent template image is
 * written whose dimensions exactly match the item's Box2D bounding box
 * at SPRITE_SCALE * PIXELS_PER_METER resolution, with the fixture
 * shapes drawn as semi-transparent guides.
 *
 * The artist can then open the template in any pixel editor, draw over
 * the guide shapes, save, and on next launch the custom art is loaded.
 *
 * @authors Charles Adair, Claude Opus 4.6 (1M context)
 * @date 4/13/2026
 */
#ifndef SPRITECACHE_H
#define SPRITECACHE_H

#include <QPixmap>
#include <QString>
#include <map>
#include "itemregistry.h"

struct SpriteEntry {
    QPixmap pixmap;
    ItemAABB aabb;
    bool valid = false;
};

class SpriteCache {
public:
    static constexpr int   SPRITE_SCALE = 4;     // 4x resolution multiplier
    static constexpr float EFFECTIVE_PPM = 50.0f * SPRITE_SCALE; // 200 px/m

    /// Load sprites for every item from Qt resources.  Call once at startup.
    static void init();

    /// Returns the sprite entry for an item, or an invalid entry.
    static const SpriteEntry& get(const QString& itemName);

    /// Returns true if a valid sprite is cached for this item.
    static bool has(const QString& itemName);

private:
    static std::map<QString, SpriteEntry> cache_;
};

#endif // SPRITECACHE_H
