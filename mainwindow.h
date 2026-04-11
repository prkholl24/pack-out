#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "level.h"
#include "player.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
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

private:
    Ui::MainWindow *ui;

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
