/**
 * @file spritecache.cpp
 * @brief Implementation of the sprite cache and template generator.
 *
 * @authors Charles Adair, Claude Opus 4.6 (1M context)
 * @date 4/13/2026
 */
#include "spritecache.h"
#include <QDir>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <cmath>

std::map<QString, SpriteEntry> SpriteCache::cache_;

// ---------------------------------------------------------------------------
//  Public API
// ---------------------------------------------------------------------------

void SpriteCache::init(const QString& spritesDir)
{
    ItemRegistry::ensureLoaded();
    QDir().mkpath(spritesDir);

    // Collect every item name across all categories.
    QStringList allNames;
    allNames << ItemRegistry::getFoodNames()
             << ItemRegistry::getDrinkNames()
             << ItemRegistry::getGearNames();

    for (const QString& name : allNames) {
        QString path = spritesDir + "/" + name + ".png";

        // Retrieve shape data for AABB and template generation.
        QString cat = ItemRegistry::categoryOf(name);
        ItemStats stats;
        if      (cat == "food")  stats = ItemRegistry::getFoodStats(name);
        else if (cat == "drink") stats = ItemRegistry::getDrinkStats(name);
        else if (cat == "gear")  stats = ItemRegistry::getGearStats(name);

        ItemAABB aabb = ItemRegistry::computeAABB(stats.shape);

        // Generate template if the file doesn't exist yet.
        if (!QFile::exists(path)) {
            generateTemplate(name, path, stats.shape, aabb);
        }

        // Load the sprite (template or artist-edited).
        SpriteEntry entry;
        entry.pixmap = QPixmap(path);
        entry.aabb   = aabb;
        entry.valid  = !entry.pixmap.isNull();

        if (!entry.valid) {
            qWarning() << "SpriteCache: failed to load sprite for" << name;
        }

        cache_[name] = std::move(entry);
    }

    qInfo() << "SpriteCache: loaded" << cache_.size() << "sprites from" << spritesDir;
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

// ---------------------------------------------------------------------------
//  Template generation
// ---------------------------------------------------------------------------

void SpriteCache::generateTemplate(const QString& itemName,
                                   const QString& filePath,
                                   const ItemShape& shape,
                                   const ItemAABB& aabb)
{
    // Image dimensions: AABB in pixels + 1px padding on each side.
    int pixelW = static_cast<int>(std::ceil(aabb.width()  * EFFECTIVE_PPM)) + 2;
    int pixelH = static_cast<int>(std::ceil(aabb.height() * EFFECTIVE_PPM)) + 2;

    // Minimum size so tiny items are still openable in an editor.
    pixelW = std::max(pixelW, 8);
    pixelH = std::max(pixelH, 8);

    QImage image(pixelW, pixelH, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter p(&image);
    p.setRenderHint(QPainter::Antialiasing);

    // Transform: Box2D local coords → image pixels.
    // Pixel (1, 1) corresponds to body-local (aabb.minX, aabb.maxY) because
    // the image Y-axis points down while Box2D Y-axis points up.
    // So:  px_x = (bx - aabb.minX) * EFFECTIVE_PPM + 1
    //      px_y = (aabb.maxY - by) * EFFECTIVE_PPM + 1
    p.translate(1.0 - aabb.minX * EFFECTIVE_PPM,
                1.0 + aabb.maxY * EFFECTIVE_PPM);
    p.scale(EFFECTIVE_PPM, -EFFECTIVE_PPM); // Y-flip: image Y-down → Box2D Y-up

    // Guide colors derived from the item's configured color.
    QColor fillColor = shape.color;
    fillColor.setAlpha(76);   // ~30%
    QColor outlineColor = shape.color;
    outlineColor.setAlpha(204); // ~80%

    QPen pen(outlineColor, 0); // cosmetic 1px pen (width 0 = cosmetic)
    p.setPen(pen);
    p.setBrush(fillColor);

    // Draw each fixture shape.
    for (const ItemFixture& fix : shape.fixtures) {
        if (fix.isBox) {
            QRectF rect(fix.centerX - fix.halfW, fix.centerY - fix.halfH,
                        fix.halfW * 2.0f, fix.halfH * 2.0f);
            p.drawRect(rect);
        } else {
            QPolygonF poly;
            for (const auto& [vx, vy] : fix.vertices) {
                poly << QPointF(vx, vy);
            }
            p.drawPolygon(poly);
        }
    }

    p.end();
    image.save(filePath, "PNG");
    qInfo() << "SpriteCache: generated template" << filePath
            << "(" << pixelW << "x" << pixelH << ")";
}
