/**
 * @file mainwindow.h
 * @brief Top-level application window and scene switcher.
 *
 * MainWindow owns a QStackedWidget that holds the game's scene
 * widgets. Currently the only scene is BackpackScene (the packing
 * phase).  As the game grows, new scenes (trip, grading, menu, etc.)
 * will be added to the stack and MainWindow will swap between them
 * in response to signals from each scene.
 *
 * ## Scene-switching pattern
 *
 * Qt's QStackedWidget displays exactly one child widget at a time.
 * Adding a widget with `addWidget()` appends it to the stack;
 * `setCurrentWidget()` makes it the visible one.  Switching scenes
 * is a single call — no manual show/hide management needed.
 *
 * MainWindow connects to each scene's completion signal (e.g.
 * BackpackScene::levelHasStarted) and calls setCurrentWidget() to
 * transition.  The scenes themselves know nothing about each other;
 * MainWindow is the only place that knows the overall flow.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed, Claude Opus 4.6 (1M context)
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
class BriefView;
class DebriefView;
class Level;

/**
 * @class MainWindow
 * @brief The main application window and scene switcher.
 *
 * Manages window geometry (fixed 800 x 600, no menu/status bars),
 * hosts a QStackedWidget as the central widget, and wires scene-
 * transition signals to stack switches.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window.
     *
     * Locks the window size to 800 x 600, hides the menu and status
     * bars, creates the QStackedWidget, and adds the BackpackScene
     * as the first (and currently only) scene.
     *
     * @param parent  Parent widget (typically nullptr for the top-level window).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroys the main window and its UI form.
     *
     * Child widgets (the stack and its scenes) are destroyed
     * automatically by Qt's parent-child ownership.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Switches to the BriefView.
     *
     * Shows the pre-level briefing with trip information.
     */
    void showBriefView();

    /**
     * @brief Switches to the BackpackScene.
     *
     * Shows the packing mini-game.
     */
    void showBackpackScene();

    /**
     * @brief Switches to the DebriefView.
     *
     * Shows the post-level results and feedback.
     */
    void showDebriefView();

private:
    /** @brief Auto-generated form from mainwindow.ui. */
    Ui::MainWindow* ui;

    /**
     * @brief Stack that holds every scene widget. Only one is
     *        visible at a time.
     *
     * QStackedWidget is the central widget of this QMainWindow.
     * Scenes are added with addWidget() and swapped with
     * setCurrentWidget().
     */
    QStackedWidget* sceneStack;

    /**
     * @brief The packing-phase scene (owned by the stack).
     *
     * When BackpackScene emits levelHasStarted(), MainWindow will
     * swap to the DebriefView via sceneStack->setCurrentWidget().
     */
    BackpackScene* backpackScene;

    /**
     * @brief The pre-level briefing view (owned by the stack).
     *
     * When BriefView emits startPackingClicked(), MainWindow swaps
     * to the BackpackScene.
     */
    BriefView* briefView;

    /**
     * @brief The post-level results view (owned by the stack).
     *
     * Provides navigation to retry, advance, or return to menu.
     */
    DebriefView* debriefView;

    /**
     * @brief The current level being played.
     *
     * Stores level data for use across views (brief, packing, debrief).
     */
    Level* currentLevel;
};
#endif // MAINWINDOW_H
