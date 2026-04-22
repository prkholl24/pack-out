#ifndef SURVIVALSCRAPBOOK_H
#define SURVIVALSCRAPBOOK_H

#include <QString>
#include <QHideEvent>
#include <QShowEvent>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <set>
#include <vector>
#include "levelresult.h"

class SurvivalScrapbook : public QWidget
{
    Q_OBJECT
public:
    explicit SurvivalScrapbook(QWidget *parent = nullptr);

    /**
     * @brief Rebuilds the book's pages from the given set of unlocked lessons.
     *
     * Page 0 remains the title page; one page per lesson follows, loaded
     * from lessons.json via LevelReportBuilder::getLessonByKey. Lessons
     * that resolve to NONE or have no JSON entry are skipped.
     */
    void setLessons(const std::set<ScienceLesson>& lessons);

signals:
    void visibilityChanged(bool visible);

public slots:
    void toggleVisibility();

private slots:
    void onLeftClicked();
    void onRightClicked();

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    void updateArrowState();

    QPushButton *leftButton;
    QPushButton *rightButton;
    QPushButton *closeButton;
    QLabel *textDisplay;
    QLabel *pageCountLabel;

    std::vector<QString> pages;
    int currentPage;
    const QString titlePage = "The Survival Handbook";
};

#endif // SURVIVALSCRAPBOOK_H
