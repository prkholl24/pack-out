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
#include "physicsObjects.h"

//Fowards Declaration
struct PhysicsObjectDef;

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

    /**
     * @brief Updates x and y position of the player icon.
     * @param int x, x coordinate
     * @param int y, y coordinate
     */
    void bodyDisplay(float x, float y);

    /**
     * @brief Slot receives the current level the player is on.
     * @param int: level, what level the user is on.
     */
    void currentLevel(int level);

    /**
     * @brief loadVisualObjectsSlot Loads the objects from the physics engine
     *        into a vector of objects that can be drawn.
     * @param objectsVector Vector of objects from the physics engine
     *        to be parsed into drawable objects
     */
    void loadVisualObjectsSlot(QVector<PhysicsObjectDef>& objectsVector);

signals:


    //Signal which is heard by setTargetFromMouse slot
    //in the physics engine.
    void updatePhysicsMousePosition(QPointF position);

    void leftMouseButtonClickedSignal(bool clicked);


private:


    //Stores information relating to visual objects
    struct VisualObjectDef{
        QString shapeType;
        float x;
        float y;
        float width;
        float height;
        float angle;
        QVector<int> color;
    };


    QVector<VisualObjectDef> visualVector;

    /**
     * @brief player's current level
     */
    int playerLevel;

    /**
     * @brief vector of FilePaths
     */
    QVector<QString> imageFilePaths;

    //Stores the background image of the widget
    QPixmap backgroundPixmap;


    /**
     * @brief ui
     */
    Ui::TrekView *ui;
    TrekBox2DPhysics* phys = new TrekBox2DPhysics(this);
    float boxX;
    float boxY;
protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // TREKVIEW_H
