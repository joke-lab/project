#include "com_set.h"
#include "ui_com_set.h"

com_set::com_set(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::com_set)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    foreach (const  QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        /*
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        qDebug() << "Serial Number: " << info.serialNumber();
        qDebug() << "System Location: " << info.systemLocation();*/
        //serial.setPort(info);
        ui->combox1_1->addItem(info.portName());
        ui->combox3_1->addItem(info.portName());
    }
    //从初始文件中读出各个参数默认值，并显示在各个下拉框内
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    //QString baud1 = ini->value("/gps_com/baud").toString(); //从初始文件中读取出最大可观测强度值
    //QString bit1 = ini->value("/gps_com/bit").toString();
    //QString parity1 = ini->value("/gps_com/parity").toString();
    //QString stop1 = ini->value("/gps_com/stop").toString();
    if(ini->value("/gps_com/check").toInt())
    {
        ui->com1_1->setChecked(true);
    }
    if(ini->value("/out_com/check").toInt())
    {
        ui->com3_1->setChecked(true);
    }
    ui->combox1_2->setCurrentText(ini->value("/gps_com/baud").toString());
    ui->combox1_3->setCurrentText(ini->value("/gps_com/bit").toString());
    ui->combox1_4->setCurrentText(ini->value("/gps_com/parity").toString());
    ui->combox1_5->setCurrentText(ini->value("/gps_com/stop").toString());
    ui->combox3_2->setCurrentText(ini->value("/out_com/baud").toString());
    ui->combox3_3->setCurrentText(ini->value("/out_com/bit").toString());
    ui->combox3_4->setCurrentText(ini->value("/out_com/parity").toString());
    ui->combox3_5->setCurrentText(ini->value("/out_com/stop").toString());
    QObject::connect(ui->save,SIGNAL(clicked(bool)),this,SLOT(save_Slot()));
    QObject::connect(ui->cancel,SIGNAL(clicked(bool)),this,SLOT(cancel_Slot()));

}

//保存按钮的槽函数
void com_set::save_Slot()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    if(ui->com1_1->isChecked())
        ini->setValue("/gps_com/check","1");
    else
        ini->setValue("/gps_com/check","0");
    if(ui->com3_1->isChecked())
        ini->setValue("/out_com/check","1");
    else
        ini->setValue("/out_com/check","0");

    ini->setValue("/gps_com/port",ui->combox1_1->currentText());
    ini->setValue("/gps_com/baud",ui->combox1_2->currentText());
    ini->setValue("/gps_com/bit",ui->combox1_3->currentText());
    ini->setValue("/gps_com/parity",ui->combox1_4->currentText());
    ini->setValue("/gps_com/stop",ui->combox1_5->currentText());
    ini->setValue("/out_com/port",ui->combox3_1->currentText());
    ini->setValue("/out_com/baud",ui->combox3_2->currentText());
    ini->setValue("/out_com/bit",ui->combox3_3->currentText());
    ini->setValue("/out_com/parity",ui->combox3_4->currentText());
    ini->setValue("/out_com/stop",ui->combox3_5->currentText());
    this->close();
}

//取消按钮的槽函数
void com_set::cancel_Slot()
{
    this->close();
}

com_set::~com_set()
{
    delete ui;
}
