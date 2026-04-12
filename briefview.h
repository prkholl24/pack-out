/**
 * @file briefview.h
 * @brief Pre-level briefing widget displaying authored trip descriptions.
 *
 * BriefView shows the player information about an upcoming level
 * before they start packing. It displays an authored mission
 * description along with a few broad trip details.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.5
 * @date 4/10/2026
 */
#ifndef BRIEFVIEW_H
#define BRIEFVIEW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class BriefView;
}
QT_END_NAMESPACE

class Level;

/**
 * @class BriefView
 * @brief Pre-level briefing widget.
 *
 * Displays authored mission text and a few broad trip details
 * before the packing phase begins. Intended to be hosted inside
 * MainWindow's QStackedWidget.
 */
class BriefView : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the briefing view.
     *
     * @param parent Owner widget (typically the QStackedWidget
     *               inside MainWindow).
     */
    explicit BriefView(QWidget *parent = nullptr);

    /**
     * @brief Destroys the briefing view and its UI form.
     */
    ~BriefView();

    /**
     * @brief Populates the view with data from a Level.
     *
     * Reads the authored level description plus broad trip metadata
     * such as duration, terrain, and activity type.
     *
     * @param level The level to display information for.
     * @param trekNumber The sequence number of the current trek.
     */
    void setLevel(const Level& level, int trekNumber);

signals:
    /**
     * @brief Emitted when the player clicks "Start Packing".
     *
     * MainWindow should connect this to transition to BackpackScene.
     */
    void startPackingClicked();

private:
    /** @brief Auto-generated form from briefview.ui. */
    Ui::BriefView* ui;
};

#endif // BRIEFVIEW_H
