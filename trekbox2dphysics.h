#ifndef TREKBOX2DPHYSICS_H
#define TREKBOX2DPHYSICS_H

#include <QObject>
#include <Box2D/Box2D.h>
#include <QTimer>
#include "physicsObjects.h"
#include <QPointF>
#include <QCoreApplication>


class TrekBox2DPhysics : public QObject
{
    Q_OBJECT
public:
    explicit TrekBox2DPhysics(QObject *parent = nullptr);
    ~TrekBox2DPhysics();
    /**
     * @brief Box2D World
     */
    b2World world;

    b2Vec2 targetPoint;

    bool hasTarget = false;

    //How many pixels mark one in game meter
    float pixelsPerMeter = 20.0f;

    //Gain on the force applied based on the distance
    //From player to mouse
    float forceGain = 4.0f;

    //Maximum force that can be applied to hiker
    float maxForce = 10.0f;
    /**
     * @brief body
     */
    b2Body* body = nullptr;

    /**
     * @brief timer
     */
    QTimer* timer;

private:
    //True if you want to load from a file at FilePath.
    bool loadFromFile = false;
    //True if you want to save createPhysicsStructure to file
    //Mainly used for initial level design.
    bool saveToFile = false;
    //True enables the world editor to create levels.
    bool worldEdit = false;

    QVector<bool> levelCompleted;

    //X and Y starting location of the player object
    //Stores the player's starting b2word (x,y) coordinates METERS BRUHH
    QPoint playerStartingPoint = QPoint(-46,-25);

    //Sets the end area
    // top left(x,y) bottom right(x,y) of end box
    QVector<int> endArea = {890,-100,960,-540};

    //World Editor Variables/Helper methods-----------
    bool objectAdded = false;

    //Value objects are moved/resized by
    int movementModifier = 5;

    //Current object color.
    QVector<int> currentColor = {255,0,0,0};

    void clearWorldBodies();

    void createEssentialWorldBodies();

    /**
     * @brief removeLastPhysicsObject: Helper method removes the last
     *        added object to the box2D World.
     */
    void removeLastPhysicsObject();

    /**
     * @brief removeAllPhysicsObjects: Helper method removes all objects
     *        from the box2D world so new ones can be made.
     */
    void removeAllPhysicsObjects();

    //------------------------------------------------


    //Vector for each objects struct
    QVector<PhysicsObjectDef> objectsVector;

    //QPair the PhysicsObjectDef with its corresponding body
    QVector<QPair<PhysicsObjectDef,b2Body*>> bodyVector;



    /**
     * @brief Turns the objects stored in the objects vector into a physical
     * objects in box2D
     */
    void extracted();

    /**
     * @brief createPhysicsObjects a method unused except for creating the levels
     * which can then later be saved to json
     */
    void createPhysicsObjects();

    /**
     * @brief Temporary to create the struct values to be drawn.
     * Then can be saved into semi permanent jsons per level.
     *
     */
    void createPhysicsStructure();

    /**
     * @brief toJsonLevel converts the physics structure to a json that can be loaded later
     *
     */
    void toJsonLevel(QString& filepath);

    /**
     * @brief fromJsonLevel creartes a physics structure from a json file
     * @param filepath
     */
    void fromJsonLevel(QString& filepath);


    //Width in pixels of the overall widget
    int widgetWidth = 1920;
    //Height in pixels of the overall widget
    int widgetHeight = 1080;

    //Maximum levels that have been made
    int maxLevel = 6;

    //Curent level the game is on
    int currentLevel = 1;



    QString FilePath = "://trekViewLevels/level" + QString::number(currentLevel) + ".json";
    QString levelBackground = "LEVEL_1";



    /**
     * @brief Converts a point on the screen to a point in box2D based on the
     *        screen size.
     *        Method was acquired from ChatGPT GPT5.3
     */
    b2Vec2 screenToWorld(const QPointF& p) const;

    /**
     * @brief worldToScreen Convert a point in the box2D world to a point on the screen
     *        Method was acquired from ChatGPT5.3
     * @param
     * @return
     */
    QPointF worldToScreen(const b2Vec2& p) const;

    /**
     * @brief checkEndArea check the end area for player coordinates
     * @return
     */
    bool checkEndArea(int x, int y);



signals:

    /**
     * @brief sendBodyLocation: Sends x and y of the player body location
     * @param x: Player x coordinate
     * @param y: Player y coordinate
     */
    void sendBodyLocation(float x, float y);

    /**
     * @brief toggleSkipButtonSignal: Toggles the views skip button
     */
    void toggleSkipButtonSignal();

    /**
     * @brief getColor: Prompts view to get a color from user
     */
    void getColorSignal();

    //sends live b2world body information
    void loadVisualObjectsSignal(QVector<PhysicsObjectDef>& objectsVector);
    //Emits the current player level
    void sendCurrentLevelSignal(int level);
    //Emits a signal to advance to the next program scene
    void moveToNextFrame();
public slots:

    /**
     * @brief worldEditorSlot: A simple world editor to easily add objects to the world
     */
    void worldEditorSlot(int key);

    /**
     * @brief setColorSlot: Sets the object color to the color sent
     * @param color: Color expressed as a vector
     */
    void setColorSlot(QVector<int> color);

    /**
     * @brief setTargetFromMouse: Sets the target point for the player based on the mouse position
     * @param mousePos: Mouse position in screen coordinates
     */
    void setTargetFromMouse(const QPointF& mousePos);

    /**
     * @brief updateWorldSlot:: Updates the box2D world and passed visual values to view to be drawn
     */
    void updateWorldSlot();

    /**
     * @brief leftMouseButtonClickedSlot: Called when the LMB is released
     */
    void leftMouseButtonClickedSlot();

    /**
     * @brief nextLevelSlot: Advances to the level passed as an int
     *        or goes to next level if levelNo is 0.
     * @param levelNo: What level to set to.
     */
    void nextLevelSlot(int levelNo, bool timerState);
};

#endif // TREKBOX2DPHYSICS_H
