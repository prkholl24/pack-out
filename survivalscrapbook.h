/**
 * @file SurvivalScrapbook
 * @brief Contains the contents of the book that the user will consult while packing their bag.
 *
 * The Survival Scrapbook is a handbook that will grow with knowledge as the user experiments through trial and error.
 * When the user fails due to a specific reason, the survival scrapbook will gain a new entry telling the user
 * how to avoid that specific issue.
 *
 * Team: Green Goombas
 * GitHub: https://github.com/UofU-CS3505/cs3505-assignment8-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *                  Brady Nelson, Lindsey Reese, Kent Wilkison
 * @date 4/9/2026
 */


#ifndef SURVIVALSCRAPBOOK_H
#define SURVIVALSCRAPBOOK_H

#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

// The use of AI was used to turn Survival Scrapbook into a Widget class.
class SurvivalScrapbook : public QWidget
{
    Q_OBJECT
public:
    explicit SurvivalScrapbook(QWidget *parent = nullptr);
    void setText(const QString &text);

private:
    QPushButton *leftButton;
    QPushButton *rightButton;
    QLabel *textDisplay;

    /**
     * @brief All the pages the user will be able to read. Needs to be unlocked after failing a run, of course.
     */
    std::vector<QString> pages;

    int currentPage;
    QString titlePage = "The Survival Handbook";
    QString page1 = "The average human consumes 1 liter of water when traversing flat terrain, 2 liters traversing varied terrain, and 3 liters traversing mountainous terrain.";

signals:

private slots:
    /**
     * @brief Update the text of the handbook to the previous page.
     */
    void onLeftClicked();

    /**
     * @brief Update the text of the handbook to the next page.
     */
    void onRightClicked();

};

#endif // SURVIVALSCRAPBOOK_H
