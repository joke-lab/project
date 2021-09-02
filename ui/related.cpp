#include "related.h"
#include "ui_related.h"

related::related(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::related)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
}

related::~related()
{
    delete ui;
}
