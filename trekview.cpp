#include "trekview.h"
#include "ui_trekview.h"

TrekView::TrekView(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TrekView)
    , world(b2Vec2 (0.0,-10.0f))
{
    ui->setupUi(this);
}

TrekView::~TrekView()
{
    delete ui;
}
