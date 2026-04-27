#include "survivalscrapbook.h"
#include "levelreportbuilder.h"

#include <QDebug>
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
    pageCountLabel = new QLabel(this);

    textDisplay->setAlignment(Qt::AlignCenter);
    textDisplay->setWordWrap(true);
    textDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    textDisplay->setContentsMargins(8, 8, 8, 8);

    pageCountLabel->setObjectName("pageCountLabel");
    pageCountLabel->setAlignment(Qt::AlignCenter);

    closeButton->setObjectName("closeButton");
    closeButton->setFlat(true);
    closeButton->setFixedSize(28, 28);

    leftButton->setFixedSize(44, 44);
    rightButton->setFixedSize(44, 44);
    leftButton->setCursor(Qt::PointingHandCursor);
    rightButton->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *closeRow = new QHBoxLayout;
    closeRow->setContentsMargins(0, 0, 0, 0);
    closeRow->addStretch();
    closeRow->addWidget(closeButton);

    QHBoxLayout *pageRow = new QHBoxLayout;
    pageRow->setContentsMargins(0, 0, 0, 0);
    pageRow->addWidget(leftButton, 0, Qt::AlignVCenter);
    pageRow->addWidget(textDisplay, 1);
    pageRow->addWidget(rightButton, 0, Qt::AlignVCenter);

    QHBoxLayout *footerRow = new QHBoxLayout;
    footerRow->setContentsMargins(0, 0, 0, 0);
    footerRow->addStretch();
    footerRow->addWidget(pageCountLabel);
    footerRow->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 12);
    layout->addLayout(closeRow);
    layout->addLayout(pageRow, 1);
    layout->addLayout(footerRow);
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
            font-size: 16px;
            line-height: 1.4;
            background: transparent;
        }
        #SurvivalScrapbook QLabel#pageCountLabel {
            color: #5b4a2a;
            font-size: 12px;
            font-weight: 600;
            letter-spacing: 1px;
        }
        #SurvivalScrapbook QPushButton {
            color: #2b2112;
            background: #e8d8ae;
            border: 2px solid #5b4a2a;
            border-radius: 22px;
            font-size: 18px;
            font-weight: 700;
        }
        #SurvivalScrapbook QPushButton:hover:enabled {
            background: #ddc88a;
        }
        #SurvivalScrapbook QPushButton:pressed:enabled {
            background: #c9b47a;
        }
        #SurvivalScrapbook QPushButton:disabled {
            color: #b5a577;
            background: #ece1c2;
            border-color: #b5a577;
        }
        #SurvivalScrapbook QPushButton#closeButton {
            background: transparent;
            border: none;
            border-radius: 14px;
        }
        #SurvivalScrapbook QPushButton#closeButton:hover {
            background: #e8d8ae;
        }
    )");

    resize(400, 340);
    updateArrowState();
}

void SurvivalScrapbook::setBriefDescription(const QString& description)
{
    titlePage = QStringLiteral("<b>THE SURVIVAL HANDBOOK</b><br><br>%1")
                    .arg(description.toHtmlEscaped().replace("\n", "<br>"));

    if (!pages.empty()) {
        pages[0] = titlePage;
        if (currentPage == 0) {
            textDisplay->setText(titlePage);
        }
    }
}

void SurvivalScrapbook::setLessons(const std::set<ScienceLesson>& lessons)
{
    const int previousPageCount = (int)pages.size();

    pages.clear();
    pages.push_back(titlePage);

    for (ScienceLesson lesson : lessons) {
        QString key = LevelReportBuilder::keyForLesson(lesson);
        if (key.isEmpty()) continue;
        QString entry = LevelReportBuilder::getLessonByKey(key);
        if (entry.isEmpty()) continue;
        pages.push_back(entry);
    }

    qDebug() << "[scrapbook] setLessons got" << lessons.size()
             << "entries, now has" << pages.size() << "pages";

    // If new pages arrived since last call, jump to the newest one so the
    // player sees the freshly-unlocked lesson instead of the cover.
    if ((int)pages.size() > previousPageCount) {
        currentPage = (int)pages.size() - 1;
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

void SurvivalScrapbook::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit visibilityChanged(true);
}

void SurvivalScrapbook::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    emit visibilityChanged(false);
}

void SurvivalScrapbook::updateArrowState()
{
    leftButton->setEnabled(currentPage > 0);
    rightButton->setEnabled(currentPage < (int)pages.size() - 1);
    pageCountLabel->setText(QStringLiteral("Page %1 of %2")
                                .arg(currentPage + 1)
                                .arg((int)pages.size()));
}
