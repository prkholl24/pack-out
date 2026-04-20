/**
 * @file debriefview.cpp
 * @brief Implementation of the DebriefView widget.
 *
 * Team: Green Goombas
 * github url: https://github.com/UofU-CS3505/cs3505-assignment9-GreenGoombas.git
 * @authors Charles Adair, Armando Carrillo, Parker Hollingsed,
 *          Brady Nelson, Lindsey Reese, Kent Wilkison, Claude Opus 4.5
 * @date 4/10/2026
 */
#include "debriefview.h"
#include "ui_debriefview.h"
#include "levelreportbuilder.h"
#include "theme.h"

#include <QPainter>
#include <QtGlobal>

DebriefView::DebriefView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DebriefView)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);

    connect(ui->retryButton,     &QPushButton::clicked, this, &DebriefView::retryLevelClicked);
    connect(ui->nextLevelButton, &QPushButton::clicked, this, &DebriefView::nextLevelClicked);
    connect(ui->menuButton,      &QPushButton::clicked, this, &DebriefView::returnToMenuClicked);
}

DebriefView::~DebriefView() {
    delete ui;
}

void DebriefView::setResults(const LevelResult& result) {
    // Scoring text is passed through verbatim from the evaluation engine.
    ui->gradeLabel->setText(QString::number(result.grade, 'f', 1));
    ui->gradeProgressBar->setValue(qRound(result.grade));

    lastPassed = result.passed;
    ui->statusLabel->setText(result.passed ? "PASSED" : "FAILED");
    refreshStatusColor();

    ui->feedbackTextEdit->setText(LevelReportBuilder::buildReport(result));
}

void DebriefView::applyTheme(const LevelTheme& theme) {
    themeAccent = theme.accent;

    const QString ac = themeAccent.name();

    ui->titleLabel->setStyleSheet(
        QString("background: transparent; color: #FFFFFF; letter-spacing: 6px;"));

    // Score card — white surface, accent border, accent text on labels.
    ui->scoreCard->setStyleSheet(QString(
        "QWidget#scoreCard { background: #FFFFFF; border: 3px solid %1; "
        "border-radius: 24px; }"
        "QLabel { background: transparent; color: #2A2A2A; }"
        ).arg(ac));
    ui->gradeTitleLabel->setStyleSheet(
        QString("color: %1; letter-spacing: 6px;").arg(ac));
    ui->gradeLabel->setStyleSheet(QString("color: %1;").arg(ac));

    ui->gradeProgressBar->setStyleSheet(QString(
        "QProgressBar { background: #E2D9C2; border: 2px solid %1; border-radius: 18px; }"
        "QProgressBar::chunk { background: %1; border-radius: 16px; }"
        ).arg(ac));

    ui->feedbackTitleLabel->setStyleSheet(
        QString("background: transparent; color: %1;").arg(ac));
    ui->feedbackTextEdit->setStyleSheet(QString(
        "QTextEdit { background: #FFFDF8; color: #2A2A2A; "
        "border: 3px solid %1; border-radius: 18px; padding: 28px; }"
        ).arg(ac));

    const QString buttonStyle = QString(
        "QPushButton { background: %1; color: #FFFFFF; border: none; "
        "border-radius: 40px; padding: 16px 40px; font-weight: 700; }"
        "QPushButton:hover { background: %2; }"
        "QPushButton:pressed { background: %3; }"
        ).arg(ac,
              themeAccent.lighter(120).name(),
              themeAccent.darker(120).name());
    ui->retryButton->setStyleSheet(buttonStyle);
    ui->nextLevelButton->setStyleSheet(buttonStyle);

    ui->menuButton->setStyleSheet(QString(
        "QPushButton { background: transparent; color: #FFFFFF; "
        "border: 3px solid #FFFFFF; border-radius: 40px; padding: 13px 36px; "
        "font-weight: 700; }"
        "QPushButton:hover { background: rgba(255,255,255,30); }"
        ));

    refreshStatusColor();
    update();
}

void DebriefView::refreshStatusColor() {
    const QString color = lastPassed ? "#2F8F4A" : "#C4392B";
    ui->statusLabel->setStyleSheet(QString("color: %1; letter-spacing: 8px;").arg(color));
}

void DebriefView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), QColor("#F4EFE4"));

    p.fillRect(QRect(0, 0, width(), 170), themeAccent);
    p.fillRect(QRect(0, 170, width(), 4), themeAccent.darker(140));

    p.fillRect(QRect(0, 990, width(), 90), themeAccent.darker(120));
}
