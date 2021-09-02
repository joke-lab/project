#include "region_selection.h"
#include "ui_region_selection.h"

region_selection::region_selection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::region_selection)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    buttongroup->addButton(ui->radio1,0); //将两个单选框加入单选框组中
    buttongroup->addButton(ui->radio2,1);
    buttongroup->addButton(ui->radio3,2);

    ini_initial(); //调用初始化函数，将界面控件恢复到上次关闭时的状态和值

    QObject::connect(ui->save,SIGNAL(clicked(bool)),this,SLOT(saveSlot())); //保存按钮对应的槽函数
    QObject::connect(ui->cancel,SIGNAL(clicked(bool)),this,SLOT(cancelSlot())); //取消按钮对应的槽函数
    QObject::connect(ui->add,SIGNAL(clicked(bool)),this,SLOT(addSlot())); //添加按钮对应的槽函数
    QObject::connect(ui->remove,SIGNAL(clicked(bool)),this,SLOT(removeSlot()));
    QObject::connect(ui->radio1,SIGNAL(clicked(bool)),this,SLOT(radiobuttonSlot())); //单选框状态改变时打开槽函数
    QObject::connect(ui->radio2,SIGNAL(clicked(bool)),this,SLOT(radiobuttonSlot())); //单选框状态改变时打开槽函数
    QObject::connect(ui->radio3,SIGNAL(clicked(bool)),this,SLOT(radiobuttonSlot())); //单选框状态改变时打开槽函数

}

//当打开界面时，用于初始化界面控件状态和值的函数
void region_selection::ini_initial()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int id = ini->value("/area/id").toInt(); //取出id号，以此判断选择哪个单选框
    //根据id号设置各个编辑框的可编辑状态
    switch(id)
    {
    case 0:
        ui->radio1->setChecked(true);
        ui->angle_range_min->setEnabled(true);
        ui->angle_range_max->setEnabled(true);
        ui->radio_range_min->setEnabled(true);
        ui->radio_range_max->setEnabled(true);
        ui->center_coor_angle->setEnabled(false);
        ui->center_coor_radio->setEnabled(false);
        ui->fixed_area_angle->setEnabled(false);
        ui->fixed_area_radio->setEnabled(false);
        ui->list->setEnabled(false);
        ui->add->setEnabled(false);
        ui->remove->setEnabled(false);
        break;
    case 1:
        ui->radio2->setChecked(true);
        ui->angle_range_min->setEnabled(false);
        ui->angle_range_max->setEnabled(false);
        ui->radio_range_min->setEnabled(false);
        ui->radio_range_max->setEnabled(false);
        ui->center_coor_angle->setEnabled(true);
        ui->center_coor_radio->setEnabled(true);
        ui->fixed_area_angle->setEnabled(true);
        ui->fixed_area_radio->setEnabled(true);
        ui->list->setEnabled(false);
        ui->add->setEnabled(false);
        ui->remove->setEnabled(false);
        break;
    case 2:
        ui->radio3->setChecked(true);
        ui->angle_range_min->setEnabled(false);
        ui->angle_range_max->setEnabled(false);
        ui->radio_range_min->setEnabled(false);
        ui->radio_range_max->setEnabled(false);
        ui->center_coor_angle->setEnabled(false);
        ui->center_coor_radio->setEnabled(false);
        ui->fixed_area_angle->setEnabled(false);
        ui->fixed_area_radio->setEnabled(false);
        ui->list->setEnabled(true);
        ui->add->setEnabled(true);
        ui->remove->setEnabled(true);
    default:
        break;
    }
    //将各个编辑栏的值设置为上次保存的值
    ui->angle_range_min->setText(ini->value("/area/angle1").toString());
    ui->angle_range_max->setText(ini->value("/area/angle2").toString());
    ui->radio_range_min->setText(ini->value("/area/radio1").toString());
    ui->radio_range_max->setText(ini->value("/area/radio2").toString());
    ui->center_coor_angle->setText(ini->value("/area/center_angle").toString());
    ui->center_coor_radio->setText(ini->value("/area/center_radio").toString());
    ui->fixed_area_angle->setText(ini->value("/area/angle_range").toString());
    ui->fixed_area_radio->setText(ini->value("/area/radio_range").toString());
}

//当单选框的状态改变时对应的槽函数
void region_selection::radiobuttonSlot()
{
    switch (buttongroup->checkedId())
    {
    case 0:
        ui->radio1->setChecked(true);
        ui->angle_range_min->setEnabled(true);
        ui->angle_range_max->setEnabled(true);
        ui->radio_range_min->setEnabled(true);
        ui->radio_range_max->setEnabled(true);
        ui->center_coor_angle->setEnabled(false);
        ui->center_coor_radio->setEnabled(false);
        ui->fixed_area_angle->setEnabled(false);
        ui->fixed_area_radio->setEnabled(false);
        ui->list->setEnabled(false);
        //ui->add->setEnabled(false);
        //ui->remove->setEnabled(false);
        break;
    case 1:
        ui->radio2->setChecked(true);
        ui->angle_range_min->setEnabled(false);
        ui->angle_range_max->setEnabled(false);
        ui->radio_range_min->setEnabled(false);
        ui->radio_range_max->setEnabled(false);
        ui->center_coor_angle->setEnabled(true);
        ui->center_coor_radio->setEnabled(true);
        ui->fixed_area_angle->setEnabled(true);
        ui->fixed_area_radio->setEnabled(true);
        ui->list->setEnabled(false);
        //ui->add->setEnabled(false);
        //ui->remove->setEnabled(false);
        break;
    case 2:
        ui->radio3->setChecked(true);
        ui->angle_range_min->setEnabled(false);
        ui->angle_range_max->setEnabled(false);
        ui->radio_range_min->setEnabled(false);
        ui->radio_range_max->setEnabled(false);
        ui->center_coor_angle->setEnabled(false);
        ui->center_coor_radio->setEnabled(false);
        ui->fixed_area_angle->setEnabled(false);
        ui->fixed_area_radio->setEnabled(false);
        ui->list->setEnabled(true);
        ui->add->setEnabled(true);
        ui->remove->setEnabled(true);
    default:
        break;
    }
}

//保存按钮对应的槽函数
void region_selection::saveSlot()
{
    //QSettings *color_ini = new QSettings(QCoreApplication::applicationDirPath()+"/color_number.ini",QSettings::IniFormat);
    /*
    if(angle_range_min->text().toInt()>360||angle_range_max->text().toInt()>360||angle_range_min->text().toInt()<0||angle_range_max->text().toInt()<0)
    {
        //当自定义的角度最小值或最大值大于360度时，当两个角度相等时，输出此提示框
        QMessageBox::warning(nullptr,"提示" ,"自定义的输入角度不正确，请重新输入",QMessageBox::Ok,0);
    }
    else if(radio_range_min->text().toInt()<color_ini->value("/minimum_observation_radius/minimum").toInt()||radio_range_min->text().toInt()>=radio_range_max->text().toInt())
    {
        //当自定义的半径最小值小于可观测范围的最小值时，当自定义的半径最小值大于等于半径最大值时，输出此提示框
        QMessageBox::warning(nullptr,"提示" ,"自定义的输入半径范围不正确，请重新输入",QMessageBox::Ok,0);
    }
    else if (color_ini->value("/maximum_observation_radius/maximum").toInt()<radio_range_max->text().toInt())
    {
        //当自定义的半径最大值大于可观测范围的最大值时，输出此提示框
        QMessageBox::warning(nullptr,"提示" ,"输入半径范围超过最大可观测范围，请重新输入",QMessageBox::Ok,0);
    }
    else if(center_coor_angle->text().toInt()>360||center_coor_radio->text().toInt()>color_ini->value("/maximum_observation_radius/maximum").toInt())
    {
        //当选取的中心点的角度大于360度或半径大于最大可观测范围时，输出此提示框
        QMessageBox::warning(nullptr,"提示" ,"选取的中心点坐标不正确，请重新输入",QMessageBox::Ok,0);
    }
    else if(fixed_area_angle->text().toInt()>360||(fixed_area_radio->text().toInt()/2)+center_coor_radio->text().toInt()>color_ini->value("/maximum_observation_radius/maximum").toInt())
    {
        //当选择的观测角度范围大于360度时，当选择的中心点的半径加上观测半径一半的大小超过最大可观测范围时，输出此提示框
        QMessageBox::warning(nullptr,"提示" ,"选取观测范围不正确，请重新输入",QMessageBox::Ok,0);
    }
    else if(center_coor_radio->text().toInt()-(fixed_area_radio->text().toInt()/2)<color_ini->value("/minimum_observation_radius/minimum").toInt())
    {
        //当选择的中心点的半径减去观测半径的一半的大小小于最小可观测范围时，输出此提示框
        QMessageBox::warning(nullptr,"提示" ,"选取观测范围不正确，请重新输入",QMessageBox::Ok,0);
    }
    else
    {*/
        //所有输入无误时，将当前界面上的一切信息及状态存入ini文件中
        QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        ini->setValue("/area/id",QString::number(buttongroup->checkedId()));
        ini->setValue("/area/angle1",ui->angle_range_min->text());
        ini->setValue("/area/angle2",ui->angle_range_max->text());
        ini->setValue("/area/radio1",ui->radio_range_min->text());
        ini->setValue("/area/radio2",ui->radio_range_max->text());
        ini->setValue("/area/center_angle",ui->center_coor_angle->text());
        ini->setValue("/area/center_radio",ui->center_coor_radio->text());
        ini->setValue("/area/angle_range",ui->fixed_area_angle->text());
        ini->setValue("/area/radio_range",ui->fixed_area_radio->text());
        emit area_change_Slot(); //发送信号给主界面，以改变主界面上的可观测范围的值
        this->close();
    //}
}

//取消按钮对应的槽函数
void region_selection::cancelSlot()
{
    this->close();
}

//添加按钮对应的槽函数
void region_selection::addSlot()
{
    regionchoose->show();
}

//减少按钮对应的槽函数
void region_selection::removeSlot()
{

}

region_selection::~region_selection()
{
    delete ui;
}
