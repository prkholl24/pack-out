#include "survivalscrapbook.h"
#include "levelreportbuilder.h"

#include <QVBoxLayout>

// The use of Claude AI was utilized to generate the visual for this Widget.
SurvivalScrapbook::SurvivalScrapbook(QWidget* parent) : QWidget(parent)
{
    setObjectName("SurvivalScrapbook");
    setAttribute(Qt::WA_StyledBackground, true);

    pages = { titlePage };
    currentPage = 0;

    leftButton  = new QPushButton("◀", this);
    rightButton = new QPushButton("▶", this);
    closeButton = new QPushButton("✕", this);
    textDisplay = new QLabel(pages[0], this);

    textDisplay->setAlignment(Qt::AlignCenter);
    textDisplay->setWordWrap(true);
    textDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    textDisplay->setContentsMargins(8, 8, 8, 8);

    leftButton->setFlat(true);
    rightButton->setFlat(true);
    closeButton->setFlat(true);

    leftButton->setFixedWidth(40);
    rightButton->setFixedWidth(40);
    closeButton->setFixedSize(28, 28);

    leftButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    rightButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QHBoxLayout *closeRow = new QHBoxLayout;
    closeRow->setContentsMargins(0, 0, 0, 0);
    closeRow->addStretch();
    closeRow->addWidget(closeButton);

    QHBoxLayout *pageRow = new QHBoxLayout;
    pageRow->setContentsMargins(0, 0, 0, 0);
    pageRow->addWidget(leftButton);
    pageRow->addWidget(textDisplay);
    pageRow->addWidget(rightButton);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 12);
    layout->addLayout(closeRow);
    layout->addLayout(pageRow);
    setLayout(layout);

    connect(leftButton,  &QPushButton::clicked, this, &SurvivalScrapbook::onLeftClicked);
    connect(rightButton, &QPushButton::clicked, this, &SurvivalScrapbook::onRightClicked);
    connect(closeButton, &QPushButton::clicked, this, &QWidget::hide);

    setStyleSheet(R"(
        #SurvivalScrapbook {
            background: #f5ecd7;
            border: 2px solid #5b4a2a;
            border-radius: 10px;
        }
        #SurvivalScrapbook QLabel {
            color: #2b2112;
            font-size: 14px;
            background: transparent;
        }
        #SurvivalScrapbook QPushButton {
            color: #2b2112;
            background: transparent;
            border: none;
            font-size: 18px;
        }
        #SurvivalScrapbook QPushButton:hover:enabled {
            color: #5b4a2a;
        }
        #SurvivalScrapbook QPushButton:disabled {
            color: #b5a577;
        }
    )");

    setFixedSize(400, 340);
    updateArrowState();
}

void SurvivalScrapbook::setLessons(const std::set<ScienceLesson>& lessons)
{
    pages.clear();
    pages.push_back(titlePage);

    for (ScienceLesson lesson : lessons) {
        QString key = LevelReportBuilder::keyForLesson(lesson);
        if (key.isEmpty()) continue;
        QString entry = LevelReportBuilder::getLessonByKey(key);
        if (entry.isEmpty()) continue;
        pages.push_back(entry);
    }

    if (currentPage >= (int)pages.size()) currentPage = (int)pages.size() - 1;
    if (currentPage < 0) currentPage = 0;
    textDisplay->setText(pages[currentPage]);
    updateArrowState();
}

void SurvivalScrapbook::onLeftClicked()
{
    if (currentPage > 0) {
        textDisplay->setText(pages[--currentPage]);
        updateArrowState();
    }
}

void SurvivalScrapbook::onRightClicked()
{
    if (currentPage < (int)pages.size() - 1) {
        textDisplay->setText(pages[++currentPage]);
        updateArrowState();
    }
}

void SurvivalScrapbook::toggleVisibility()
{
    setVisible(!isVisible());
}

void SurvivalScrapbook::updateArrowState()
{
    leftButton->setEnabled(currentPage > 0);
    rightButton->setEnabled(currentPage < (int)pages.size() - 1);
}
