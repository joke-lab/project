#include "region.h"
#include "ui_region.h"

region::region(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::region)
{
    ui->setupUi(this);
}

region::~region()
{
    delete ui;
}
