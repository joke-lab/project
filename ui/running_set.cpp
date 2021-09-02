#include "running_set.h"
#include "ui_running_set.h"
/*
//running_set类是“运行设置”界面的类，包含了界面控件的布局和相应功能的实现。
//运行设置界面从上至下依次为以下几部分：网卡名称的设置、采集方式、是否自动存储原始数据的设置、
//存储路径和计算结果的存储路径的设置。
//打开界面时，各个部分的初始状态为上一次保存的状态。
//网卡名称部分，每次打开时都会检测当前电脑上的网卡物理地址，在下拉单中能看见检测到的所有地址。
//采集方式处，设置了一个radiobutton组，在手动采集、每小时平均采集和累计平均模式中必须且只能选择一种。
//手动采集模式是指通过手动点击“打开雷达”、“关闭雷达”、“开始处理”和“停止处理”来控制雷达的打开和关闭以及数据的采集。
//每小时平均采集的模式下需要选择采集次数，此模式是指在一个小时内，根据采集次数，
//自动执行“打开雷达”、“关闭雷达”、“开始处理”和“停止处理”等功能。
//累计平均模式下需要选择时间平均间隔和存储间隔，此模式是指每隔三分钟采集一组数据，根据存储间隔来存储数据，
//如，若存储间隔为1，则采集到的数据存一组，丢一组，再存一组，以此类推。时间平均间隔是指，每组数据都会算出一个结果值，
//将在间隔内计算出的结果累计，再得到出一个最终的结果。
//当选择一种模式时，其余模式下需设置的下拉框均为不可操作状态。
//一个checkbox来选择是否自动存储原始数据，当选中时，可编辑存储路径（接收到的原始数据的存储路径）和计算结果的存储路径，
//当未选中时，存储路径和计算结果的存储路径的条编辑栏和按钮都为不可操作状态。
//点击保存按钮时，将当前界面上控件的状态全部存入ini文件中，以便下次打开此界面时调用。
//当点击取消按钮时，此界面直接关闭，此次在界面上的操作均不记录。
*/

running_set::running_set(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::running_set)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    //按钮组件，将三个radiobutton加入并设定对应的序列号
    buttongroup = new QButtonGroup;
    buttongroup->addButton(ui->radioButton1,0);
    buttongroup->addButton(ui->radioButton2,1);
    buttongroup->addButton(ui->radioButton3,2);

    NICname_read(); //网卡读取函数
    qDebug() << "运行到此处";
    save_Path();  //存储路径和计算结果的存储路径的函数
    collection_mode();  //采集方式模块的函数

    QObject::connect(ui->save,SIGNAL(clicked(bool)),this,SLOT(saveSlot())); //保存按钮的信号与槽连接
    QObject::connect(ui->cancel,SIGNAL(clicked(bool)),this,SLOT(cancelSlot()));  //取消按钮的信号与槽连接
    QObject::connect(ui->radioButton1,SIGNAL(clicked(bool)),this,SLOT(radiobuttonSlot()));  //同下两个一起为采集方式处三个radiobutton的信号与槽连接
    QObject::connect(ui->radioButton2,SIGNAL(clicked(bool)),this,SLOT(radiobuttonSlot()));
    QObject::connect(ui->radioButton3,SIGNAL(clicked(bool)),this,SLOT(radiobuttonSlot()));
    QObject::connect(ui->checkBox,SIGNAL(stateChanged(int)),this,SLOT(checkbox_state_ChangedSlot()));  //自动存储原始数据选择框的信号与槽连接
    QObject::connect(ui->savePath,SIGNAL(clicked(bool)),this,SLOT(savepathSlot()));  //存储路径处按钮的信号与槽连接
    QObject::connect(ui->result_savePath,SIGNAL(clicked(bool)),this,SLOT(cal_res_savepathSlot()));  //计算结果的存储路径的信号与槽连接

}

//网卡读取函数
//使用winpcap进行网卡读取并显示
void running_set::NICname_read()
{
    pcap_if_t *alldevs;
    pcap_if_t *d;
    char errbuf[PCAP_ERRBUF_SIZE];

    //pcap_findalldevs找寻此设备上现有的网卡
    if(pcap_findalldevs(&alldevs,errbuf) == -1)
    {
        qDebug() << errbuf;
        QString errbuf_string(errbuf);
        ui->Nicname->addItem(errbuf_string); //寻找失败时，在下拉框中显示错误信息
    }
    for(d = alldevs; d; d = d->next)
    {
        qDebug() << "找到网卡";
        if(d->description)
            ui->Nicname->addItem(QString(d->name)+QString(d->description));  //找到网卡时，将网卡信息和网卡描述一起显示于下拉框内
        else
            ui->Nicname->addItem(QString(d->name));  //没有网卡描述时只显示网卡信息
    }

    //从ini文件中读取出上次选择的网卡，设置为默认选择的网卡
    QSettings *nicname_ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString current_show = nicname_ini->value("/NICname_choosen/nicname").toString();
    ui->Nicname->setCurrentText(current_show);
}
//采集方式模块恢复上次保存状态的函数
void running_set::collection_mode()
{
    QSettings *nicname_ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    //从ini文件中读出存入的采集次数设为初始值
    QString collection_number = nicname_ini->value("collection_mode/collection_times").toString();
    ui->collectionTimes->setCurrentText(collection_number);
    //从ini文件中读出存入的时间平均间隔设为初始值
    QString cycle_number = nicname_ini->value("collection_mode/cycle").toString();
    ui->cycle->setCurrentText(cycle_number);
    QString storage_interval = nicname_ini->value("collection_mode/storage_interval").toString();
    ui->storageInternal->setCurrentText(storage_interval);
    //从ini文件中读出存入的id号，由此设置初始的按钮选中状态和下拉栏的是否可操作状态
    int Id = nicname_ini->value("/collection_mode/Id").toInt();
    switch(Id)
    {
    case 0:
        ui->radioButton1->setChecked(true);
        ui->collectionTimes->setEnabled(false);
        ui->cycle->setEnabled(false);
        ui->storageInternal->setEnabled(false);
        break;
    case 1:
        ui->radioButton2->setChecked(true);
        ui->collectionTimes->setEnabled(true);
        ui->cycle->setEnabled(false);
        ui->storageInternal->setEnabled(false);
        break;
    case 2:
        ui->radioButton3->setChecked(true);
        ui->collectionTimes->setEnabled(false);
        ui->cycle->setEnabled(true);
        ui->storageInternal->setEnabled(true);
        break;
    default:
        break;
    }
    //从ini文件中读出存储帧数
    QString frame_num = nicname_ini->value("/collection_mode/framenumber").toString();
    ui->framenumber->setText(frame_num);
}

//采集方式处三个选择按键对应的槽函数
void running_set::radiobuttonSlot()
{
    //根据按钮组的Id号选择不同的模式
    switch(buttongroup->checkedId())
    {
    case 0:
        ui->collectionTimes->setEnabled(false);
        ui->cycle->setEnabled(false);
        ui->storageInternal->setEnabled(false);
        break;
    case 1:
        ui->collectionTimes->setEnabled(true);
        ui->cycle->setEnabled(false);
        ui->storageInternal->setEnabled(false);
        break;
    case 2:
        ui->collectionTimes->setEnabled(false);
        ui->cycle->setEnabled(true);
        ui->storageInternal->setEnabled(true);
        break;
    default:
        break;
    }
}

//存储路径和计算结果的存储路径的设置
void running_set::save_Path()
{
    QSettings *nicname_ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    //将当前存储路径设置为上次选择的路径
    QString savepath = nicname_ini->value("/initialPath/savepath").toString();
    ui->lineEdit1->setText(savepath);
    //将当前计算结果的存储路径设置为上次选择的路径
    QString calsavepath = nicname_ini->value("/storage_path_of_calculation_results/calsavepath").toString();
    ui->lineEdit2->setText(calsavepath);
    //将自动存储原始数据选择框的状态设置为上次设定的状态
    int judge = nicname_ini->value("/auto_store_rawdata_checkbox/check").toInt();
    ui->checkBox->setChecked(judge);
    //检查此时自动存储原始数据选择框的状态，以此设定Lineedit1和存储路径的按钮的状态
    if(!ui->checkBox->isChecked())
    {
        ui->lineEdit1->setEnabled(false);
        ui->savePath->setEnabled(false);
    }
}
//自动存储原始数据选择框的改变对应的槽函数
void running_set::checkbox_state_ChangedSlot()
{
    //当选中时设置lineedit1和存储路径的按钮可操作状态
    if(ui->checkBox->isChecked())
    {
        ui->lineEdit1->setEnabled(true);
        ui->savePath->setEnabled(true);
    }
    else if(!ui->checkBox->isChecked()) //当未选中时设置lineedit1和存储路径的按钮不可操作状态
    {
        ui->lineEdit1->setEnabled(false);
        ui->savePath->setEnabled(false);
    }
}

//存储路径处按钮对应的槽函数
void running_set::savepathSlot()
{
     QString judge = ui->lineEdit1->text();
     //判断当前lineedit的存储路径是否为空
     //为空的话点击按钮打开的为当前路径
     //不为空的话打开的是当前Lineedit中显示的存储路径位置
     if(judge.isEmpty())
     {
         QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("save path"),QDir::currentPath()));
         if(!directory.isEmpty())
         {
             ui->lineEdit1->setText(directory);
         }
     }
     else
     {
         QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("save path"),judge));
         if(!directory.isEmpty())
         {
             ui->lineEdit1->setText(directory);
         }
     }
}

//计算结果的存储路径处按钮对应的槽函数
void running_set::cal_res_savepathSlot()
{
    QString judge = ui->lineEdit2->text();
    //判断当前lineedit的存储路径是否为空
    //为空的话点击按钮打开的为当前路径
    //不为空的话打开的是当前Lineedit中显示的存储路径位置
    if(judge.isEmpty())
    {
        QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("存储路径"),QDir::currentPath()));
        if(!directory.isEmpty())
        {
            ui->lineEdit2->setText(directory);
        }
    }
    else
    {
        QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("计算结果的存储路径"),judge));
        if(!directory.isEmpty())
        {
            ui->lineEdit2->setText(directory);
        }
    }
}

//保存按钮的槽函数
void running_set::saveSlot()
{
    //保存选中的网卡数据
    QString nicname_currenttext = ui->Nicname->currentText();
    QSettings *nicname_ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    nicname_ini->setValue("/NICname_choosen/nicname",nicname_currenttext);

    //保存当前选定的存储路径
    QString savepath = ui->lineEdit1->text();
    QDir savpath_dir;
    if(!savpath_dir.exists(savepath)) //判断输入框中设定的路径是否存在
    {
        //不存在时，设定当前应用所在路径为存储路径
        qDebug() << "不存在当前设置的路径";
        nicname_ini->setValue("/initialPath/savepath",QCoreApplication::applicationDirPath());
    }
    else
    {
        nicname_ini->setValue("/initialPath/savepath",savepath); //存在时，将输入框中的路径存入ini文件中
    }

    //保存当前选定的计算结果的存储路径
    QString calsavepath = ui->lineEdit2->text();
    if(!savpath_dir.exists(calsavepath)) //判定输入框中的计算结果存储路径是否存在
    {
        //不存在时，设定当前应用所在路径为存储路径
        qDebug() << "不存在当前设置的路径";
        nicname_ini->setValue("/storage_path_of_calculation_results/calsavepath",QCoreApplication::applicationDirPath());
    }
    else
    {
        nicname_ini->setValue("/storage_path_of_calculation_results/calsavepath",calsavepath); //存在时，将输入框中的路径存入ini文件中
    }

    //保存自动存储原始数据选择框的状态
    int check_status = ui->checkBox->isChecked();
    nicname_ini->setValue("/auto_store_rawdata_checkbox/check",check_status);

    //保存采集方式处的选择状态
    //存入最终选中的按钮的Id号
    int Id = buttongroup->checkedId();
    nicname_ini->setValue("/collection_mode/Id",Id);
    //存入采集次数
    QString hourly_ave_acq = ui->collectionTimes->currentText();
    nicname_ini->setValue("collection_mode/collection_times",hourly_ave_acq);
    //存入时间平均间隔
    QString cycle_dat = ui->cycle->currentText();
    nicname_ini->setValue("/collection_mode/cycle",cycle_dat);
    //存入存储周期
    QString storage_val = ui->storageInternal->currentText();
    nicname_ini->setValue("/collection_mode/storage_interval",storage_val);
    //存入存储帧数
    QString frame_num = ui->framenumber->text();
    nicname_ini->setValue("/collection_mode/framenumber",frame_num);

    emit runset_send_to_loginformation();
    this->close();
}

//取消按钮的槽函数
void running_set::cancelSlot()
{
    this->close();
}
running_set::~running_set()
{
    delete ui;
}
