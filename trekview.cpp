#include "trekview.h"
#include "ui_trekview.h"

TrekView::TrekView(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TrekView)
{
    ui->setupUi(this);
}

TrekView::~TrekView()
{
    delete ui;
}
