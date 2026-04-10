/**
 * @file mainwindow.h
 * @brief Top-level application window and scene switcher.
 *
 * MainWindow owns a QStackedWidget that holds the game's scene widgets
 * (currently just BackpackScene). As new scenes are added (trip,
 * grading, etc.) MainWindow will switch between them in response to
 * signals from each scene.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/7/2026
 * Reviewed by Lindsey Reese 4/8/2026
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
 *
 * Manages window geometry, hosts a QStackedWidget as the central
 * widget, and wires scene-transition signals to scene switches.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    /**
     * @brief Stack that holds every scene widget. Only one is visible.
     */
    QStackedWidget* sceneStack;

    /**
     * @brief The packing-phase scene. Owned by the stack.
     */
    BackpackScene* backpackScene;
};
#endif // MAINWINDOW_H
