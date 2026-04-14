#ifndef SURVIVALSCRAPBOOK_H
#define SURVIVALSCRAPBOOK_H

#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

class SurvivalScrapbook : public QWidget
{
    Q_OBJECT
public:
    explicit SurvivalScrapbook(QWidget *parent = nullptr);
    void setText(const QString &text);

public slots:
    void toggleVisibility();  // <-- new

private:
    QPushButton *leftButton;
    QPushButton *rightButton;
    QLabel *textDisplay;

    std::vector<QString> pages;
    int currentPage;
    QString titlePage = "The Survival Handbook";
    QString page1 = "The average human consumes 1 liter of water when traversing flat terrain, 2 liters traversing varied terrain, and 3 liters traversing mountainous terrain.";

signals:

private slots:
    void onLeftClicked();
    void onRightClicked();
};

#endif // SURVIVALSCRAPBOOK_H
