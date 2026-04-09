#include "trekview.h"
#include "ui_trekview.h"
#include <QPainter>

TrekView::TrekView(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TrekView)
{
    ui->setupUi(this);
    connect(phys,&TrekBox2DPhysics::sendBodyLocation,this,&TrekView::bodyDisplay);
}

TrekView::~TrekView()
{
    delete ui;
}

void TrekView::bodyDisplay(float x, float y){
    boxX = x;
    boxY = y;
}

void TrekView::paintEvent(QPaintEvent *event){
    QPainter painter(this);

    painter.setBrush(Qt::blue);

    painter.drawRect(boxX,boxY,100,100);

    update();
}
