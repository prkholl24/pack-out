/**
 * @file
 * @brief The TrekView is a MainWindow subclass to display user's current Trek/Level simulation.
 *
 *
 * Team: Green Goombas
 * GitHub: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *                  Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/08/2026
 */
#ifndef TREKVIEW_H
#define TREKVIEW_H

#include "trekbox2dphysics.h"
#include <QWidget>
#include <Box2D/Box2D.h>

namespace Ui {
class TrekView;
}

class TrekView : public QWidget
{
    Q_OBJECT

public:
    explicit TrekView(QWidget *parent = nullptr);
    ~TrekView();

public slots:
    void bodyDisplay(float x, float y);

    /**
     * @brief Slot receives the current level the player is on.
     * @param int: level, what level the user is on.
     */
    void currentLevel(int level);
private:
    /**
     * @brief player's current level
     */
    int playerLevel;

    /**
     * @brief vector of FilePaths
     */
    QVector<QString> imageFilePaths;

    QImage backgroundImage;

    /**
     * @brief ui
     */
    Ui::TrekView *ui;
    TrekBox2DPhysics* phys = new TrekBox2DPhysics(this);
    float boxX;
    float boxY;
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // TREKVIEW_H
