/**
 * @file mainwindow.h
 * @brief Top-level application window and scene switcher.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "level.h"
#include "player.h"
#include "trekview.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QStackedWidget;
QT_END_NAMESPACE

class BackpackScene;

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
     * @brief Triggered when the player's backpack items change.
     */
    void onBackpackChanged();

    /**
     * @brief Triggered when the player's totals change.
     */
    void onStatsChanged(double weight, double volume);

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
     * @brief The trek view scene.
     */
    TrekView* trekViewScene;

    /**
     * @brief The player object managing the current session's backpack.
     */
    Player player;

    /**
     * @brief The collection of Utah treks for the player to progress through.
     */
    std::vector<Level> treks;
};
#endif // MAINWINDOW_H
