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

#include <QtGlobal>

DebriefView::DebriefView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DebriefView)
{
    ui->setupUi(this);

    // Connect navigation buttons to emit signals.
    connect(ui->retryButton, &QPushButton::clicked,
            this, &DebriefView::retryLevelClicked);
    connect(ui->nextLevelButton, &QPushButton::clicked,
            this, &DebriefView::nextLevelClicked);
    connect(ui->menuButton, &QPushButton::clicked,
            this, &DebriefView::returnToMenuClicked);
}

DebriefView::~DebriefView()
{
    delete ui;
}

void DebriefView::setResults(const LevelResult& result)
{
    // Display the grade as a number with one decimal place.
    ui->gradeLabel->setText(QString::number(result.grade, 'f', 1));

    // Update the progress bar to visualize the grade.
    ui->gradeProgressBar->setValue(qRound(result.grade));

    // Display pass/fail status.
    if (result.passed) {
        ui->statusLabel->setText("PASSED");
        ui->statusLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->statusLabel->setText("FAILED");
        ui->statusLabel->setStyleSheet("color: red; font-weight: bold;");
    }

    // Generate and display the feedback report.
    QString report = LevelReportBuilder::buildReport(result);
    ui->feedbackTextEdit->setText(report);
}
