/**
 * @file mainwindow.h
 * @brief Top-level application window and scene switcher.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <vector>
#include "level.h"
#include "player.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QStackedWidget;
QT_END_NAMESPACE

class BackpackScene;
class BriefView;
class DebriefView;
class Level;

/**
 * @class MainWindow
 * @brief The main application window and scene switcher.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window.
     * @param parent  Parent widget.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroys the main window and its UI form.
     */
    ~MainWindow();

    /**
     * @brief Adds an item to the player's current backpack.
     * @param item - The item to add.
     */
    void addPackItem(Item* item);

    /**
     * @brief Evaluates the current backpack against a trek.
     * @param trekIndex - The index of the trek to evaluate.
     */
    void evaluateTrek(int trekIndex);

private slots:
    /**
     * @brief Switches to the BriefView.
     */
    void showBriefView();

    /**
     * @brief Switches to the BackpackScene.
     */
    void showBackpackScene();

    /**
     * @brief Switches to the DebriefView.
     */
    void showDebriefView();

    /**
     * @brief Triggered when the player's backpack items change.
     */
    void onBackpackChanged();

    /**
     * @brief Triggered when the player's totals change.
     */
    void onStatsChanged(double weight, double volume);

    /**
     * @brief Adds a newly packed item to the player's model backpack.
     * @param itemName The scene-level item identifier emitted by BackpackScene.
     */
    void handlePackedItemAdded(const QString& itemName);

    /**
     * @brief Removes an unpacked item from the player's model backpack.
     * @param itemName The scene-level item identifier emitted by BackpackScene.
     */
    void handlePackedItemRemoved(const QString& itemName);

private:
    /** @brief Auto-generated form from mainwindow.ui. */
    Ui::MainWindow* ui;

    /**
     * @brief Stack that holds every scene widget.
     */
    QStackedWidget* sceneStack;

    /**
     * @brief The packing-phase scene.
     */
    BackpackScene* backpackScene;

    /**
     * @brief The pre-level briefing view.
     */
    BriefView* briefView;

    /**
     * @brief The post-level results view.
     */
    DebriefView* debriefView;

    /**
     * @brief Mirrors bag contents by scene item name so removals can
     *        delete the correct model object.
     */
    std::map<QString, std::vector<Item*>> packedItemsByName;

    /**
     * @brief The player object managing the current session's backpack.
     */
    Player player;

    /**
     * @brief The collection of Utah treks for the player to progress through.
     */
    std::vector<Level> treks;

    /**
     * @brief The index of the current trek being played.
     */
    int currentTrekIndex;

    /**
     * @brief Wires the current BackpackScene instance into MainWindow.
     */
    void connectBackpackSceneSignals();

    /**
     * @brief Recreates the packing scene so each attempt starts fresh.
     */
    void rebuildBackpackScene();

    /**
     * @brief Clears the player's current packed-item attempt state.
     */
    void resetAttemptState();

    /**
     * @brief Advances to the next available level, if any.
     */
    void advanceToNextLevel();
};
#endif // MAINWINDOW_H
