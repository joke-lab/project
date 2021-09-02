#include "system_set.h"
#include "ui_system_set.h"

system_set::system_set(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::system_set)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    ini_set(); //站点信息设置函数的调用

    QObject::connect(ui->confirm,SIGNAL(clicked(bool)),this,SLOT(confirmSlot())); //确认按钮的槽函数的连接
    QObject::connect(ui->cancel,SIGNAL(clicked(bool)),this,SLOT(cancelSlot())); //取消按钮的槽函数的连接

}

//设置界面初始信息的读出与设置
void system_set::ini_set()
{
     QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
     ui->Lineedit1->setText(ini->value("/site_information/name").toString());
     ui->Lineedit2->setText(ini->value("/site_information/englishabbr").toString());
     ui->Lineedit3->setText(ini->value("/site_information/sitenumber").toString());
     if(ini->value("/site_information/shoreBased").toInt())
         ui->check1->setChecked(true);
     else if(ini->value("/site_information/onBoard").toInt())
         ui->check2->setChecked(true);
     ui->radar_correction_angle->setText(ini->value("/calculation_parameter/radar_correction_angle").toString());
     ui->water_depth->setText(ini->value("/calculation_parameter/water_depth").toString());
     //ui->ship_high->setText(ini->value("/calculation_parameter/ship_high").toString());
     ui->time_domain_points->setText(ini->value("/calculation_parameter/time_domain_points").toString());
     ui->airspace_points_x->setText(ini->value("/calculation_parameter/airspace_points_x").toString());
     ui->airspace_points_y->setText(ini->value("/calculation_parameter/airspace_points_y").toString());
     ui->sampling_interval_time->setText(ini->value("/calculation_parameter/sampling_interval_time").toString());
     ui->sampling_interval_x->setText(ini->value("/calculation_parameter/sampling_interval_x").toString());
     ui->sampling_interval_y->setText(ini->value("/calculation_parameter/sampling_interval_y").toString());
     ui->edit1->setText(ini->value("/radar_information/minimum").toString());
     ui->edit2->setText(ini->value("/radar_information/maximum").toString());
     ui->combo->setCurrentText(ini->value("/radar_information/samplestep").toString());
     ui->edit4->setText(ini->value("/radar_information/noisebasevalue").toString());
     ui->edit5->setText(ini->value("/radar_information/triggerdelay").toString()); //触发延时
     ui->edit6->setText(ini->value("/radar_information/openglAngle").toString());
     ui->gatestep->setText(ini->value("/radar_information/gatestep").toString());
     ui->grade->setText(ini->value("/radar_information/grade").toString());
     ui->AsynDisturb->setChecked(ini->value("/radar_information/state").toBool());
     qDebug() << "初始信息调用" <<ui->grade;
}

//确认按钮的槽函数
//将此时界面上设置的参数全部存入初始文件中
void system_set::confirmSlot()
{
    if(!global::transceiver_turnon_bool)
    {
        QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        ini->setValue("/site_information/name",ui->Lineedit1->text());
        ini->setValue("/site_information/englishabbr",ui->Lineedit2->text());
        ini->setValue("/site_information/sitenumber",ui->Lineedit3->text());
        ini->setValue("/calculation_parameter/radar_correction_angle",ui->radar_correction_angle->text());
        ini->setValue("/calculation_parameter/water_depth",ui->water_depth->text());
        ini->setValue("/calculation_parameter/time_domain_points",ui->time_domain_points->text());
        ini->setValue("/calculation_parameter/airspace_points_x",ui->airspace_points_x->text());
        ini->setValue("/calculation_parameter/airspace_points_y",ui->airspace_points_y->text());
        ini->setValue("/calculation_parameter/sampling_interval_time",ui->sampling_interval_time->text());
        ini->setValue("/calculation_parameter/sampling_interval_x",ui->sampling_interval_x->text());
        ini->setValue("/calculation_parameter/sampling_interval_y",ui->sampling_interval_y->text());
        ini->setValue("/radar_information/samplestep",ui->combo->currentText());
        ini->setValue("/radar_information/minimum",ui->edit1->text()); //采样开始距离
        ini->setValue("/radar_information/maximum",ui->edit2->text()); //采样结束距离
        ini->setValue("/radar_information/noisebasevalue",ui->edit4->text()); //噪声基底
        ini->setValue("/radar_information/triggerdelay",ui->edit5->text()); //触发延时
        ini->setValue("/radar_information/openglAngle",ui->edit6->text());
        ini->setValue("/radar_information/gatestep",ui->gatestep->text());
        ini->setValue("/radar_information/grade",ui->grade->text());
        int dsamplestart = ini->value("/radar_information/minimum").toInt();
        int dsampleend = ini->value("/radar_information/maximum").toInt();
        int triggerdelay = ini->value("/radar_information/triggerdelay").toInt();
        //int samplestart = dsamplestart* 3e8 * (1.0/120000000)/2 - triggerdelay + 0.5;
        //int sampleend = dsampleend* 3e8 * (1.0/120000000)/2 - triggerdelay + 0.5;
        int startpoint = dsamplestart + triggerdelay; //计算触发延时之后起始点数
        int endpoint = dsampleend + triggerdelay; //计算触发延时之后结束点数
        ini->setValue("/radar_information/startpoint",startpoint);
        ini->setValue("/radar_information/endpoint",endpoint);

        if(ui->check1->isChecked())
        {
            ini->setValue("/site_information/shoreBased",1);
            ini->setValue("/site_information/onBoard",0);
        }
        else if (ui->check2->isChecked())
        {
            ini->setValue("/site_information/shoreBased",0);
            ini->setValue("/site_information/onBoard",1);
        }

        //反异步干扰判断
        if(ui->AsynDisturb->isChecked())
        {
            ini->setValue("/radar_information/state",1);//反异步干扰开启
        }
        else
        {
            ini->setValue("/radar_information/state",0);//反异步干扰关闭
        }
        emit update_radarinform(); //发往主界面，采样起始结束距离和采样步长
        //emit update_//改变反异步干扰状态
        this->close();
        qDebug() << "调用一次";
    }
    else if(global::transceiver_turnon_bool)
    {
        //QMessageBox::warning(nullptr,"无法更改系统设置","请先关闭发射机，再更改系统参数！",QMessageBox::Ok,1);
    }
}

//取消按钮的槽函数
void system_set::cancelSlot()
{
    this->close();
}

system_set::~system_set()
{
    delete ui;
}
