#include "playback.h"
#include "ui_playback.h"

/*
//playback类是“数据回放”界面的类，包含了界面的布局和界面上相关功能的实现
//构造函数中实现了控件的添加和界面的布局；从ini文件中读取出上次存储的信息作为界面初始的内容；四个按钮的信号与槽的连接函数
//界面上显示的有数据文件、频率、数据格式和四个按钮
//数据文件编辑框需要输入要回放的数据文件的绝对路径，可以手动输入，也可以通过打开文件按钮进行操作
//频率目前没有用上，在选择了要回放的文件后，需要为其选择对应的数据格式，否则会出现无法预估的错误
//界面一共四个按钮：打开文件、开始回放、停止回放、保存并退出
//点击打开文件按钮时，会打开数据文件后编辑栏中的绝对路径，选择要回放的数据文件，点击确认后，选择的文件的绝对路径将显示在数据文件处
//点击开始回放按钮时，会触发playback_readdata子线程开启，主界面开始不断的刷新，并关闭此界面，
//在主界面的opengl处绘制当前选择回放的数据文件，主界面的雷达信息处的信息会根据不同的数据文件进行更新
//点击停止回放按钮时，不论是在回放中或回放完毕，opengl界面回复原始状态，playback_readdata子线程中的耗时函数停止，线程处于等待状态，主界面停止刷新
//点击保存并退出按钮时，此时界面上选择的数据文件的绝对路径、频率和数据格式都将存入ini文件中，在下次打开此界面时作为初始值；最后关闭此界面
*/

int playback_static_varying = 0; //设置一个全局变量，用以判断当前回放界面的状态是开始回放还是停止回放还是保存并退出，为其进行初始化

playback::playback(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::playback)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    //从ini文件中读取出上次打开的文件
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString lineedit_initial = ini->value("/review_file/filename").toString();
    QString data_format = ini->value("/dataformat/format").toString();
    ui->lineEdit->setText(lineedit_initial); //用edit显示出记忆的文件名
    ui->combox2->setCurrentText(data_format); //在数据格式处显示出上次保存的数据格式

    QObject::connect(ui->openFile,SIGNAL(clicked(bool)),this,SLOT(openFileSlot())); //打开文件-信号与槽的链接
    QObject::connect(ui->startReview,SIGNAL(clicked(bool)),this,SLOT(startReviewSlot())); //开始回放-信号与槽的链接
    QObject::connect(ui->stopReview,SIGNAL(clicked(bool)),this,SLOT(stopReviewSlot())); //停止回放-信号与槽的链接
    QObject::connect(ui->exit,SIGNAL(clicked(bool)),this,SLOT(exitSlot())); //退出界面-信号与槽的链接

}

//打开文件的槽函数
void playback::openFileSlot()
{
    QString directory = ui->lineEdit->text(); //读取当前数据文件编辑栏中的文件绝对路径
    if(directory.isEmpty()) //若当前编辑栏中为空
    {
        filename = QFileDialog::getOpenFileName(this,"打开",QDir::currentPath()); //将执行文件所在路径设置为打开的路径
    }
    else
    {
        filename = QFileDialog::getOpenFileName(this,"打开",directory);
    }
    ui->lineEdit->setText(filename);
}

//开始回放的槽函数
void playback::startReviewSlot()
{
    playback_static_varying = 1; //更改此全局变量的值，以表示状态为开始回放
    emit start_playback(); //用以发送给主界面更新系统信息的信号
    filename = ui->lineEdit->text();
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    ini->setValue("/review_file/filename",filename);
    ini->setValue("/dataformat/format",ui->combox2->currentText());

    /*
    if(first_time_start) //判断是否是第一次点击开始回放槽函数
    {
        first_time_start = false;
        playback_readfile_thread = new QThread; //将读取数据的子线程实例化
        read_data->moveToThread(playback_readfile_thread); //将读取数据的类放入线程中
        playback_readfile_thread->start(); //开启线程
    }
    connect(read_data,&playback_readdata::started_readdata_message,this,&playback::start_readfile_received_Slot); //将读取数据线程中的返回信号与主线程中的接收的槽函数相连接
    connect(read_data,&playback_readdata::pingtan_change_distance_first,this,&playback::pingtan_change_distance_Slot); //读取平潭数据时接收信号以触发槽函数发出信号给主界面改变显示观测范围
    connect(read_data,&playback_readdata::start_showing,this,&playback::start_showing_Slot); //当读取线程中第一次进行了内存交换后调用槽函数
    connect(read_data,&playback_readdata::longkou_sampling_step,this,&playback::receive_longkou_sampling_step_Slot); //接收从数据读取中传来的龙口数据的采样步长的槽函数，用以传递信号给主界面
    //connect(read_data,&playback_readdata::start_figure,figure_data,&playback_figure::playback_figure_thread); //当读取线程第一次将计算区域内的数据存满内存并交换后，调用此槽函数触发信号，调用计算线程中的耗时函数
    connect(read_data,&playback_readdata::playback_figure_message,this,&playback::receive_playback_figure_message_Slot);
    connect(read_data,&playback_readdata::playback_figure_returnvalue,this,&playback::playback_longkou_figure); //接收来自计算线程的信号，用于向主线程中发送信号，以改变主界面上的最终计算结果
    */
    emit start_playback_readfile_thread(); //发送信号，在子线程中开始运行耗时函数
    this->close();
}

//当读取线程第一次进行了内存交换后调用的槽函数
void playback::start_showing_Slot()
{
    global::startreview_clicked = true; //开启主线程的画图
    global::run_firsttime = true;
    qDebug() << "playback中开启了opengl绘制界面的全局变量";
}

//接收来自读取数据子线程中返回信息的槽函数
void playback::start_readfile_received_Slot()
{
    qDebug() << "playback：接收到来自读取数据子线程中返回的信息";
}

void playback::receive_playback_figure_message_Slot()
{
    qDebug() << "计算子线程返回信息已表明子线程成功开启";
}

//接收从数据读取中传来的平潭数据改变观测范围的槽函数
void playback::pingtan_change_distance_Slot()
{
    emit pingtan_change_distance_second(); //发送信号给主函数，更改主界面上的观测范围
}

//接收从数据读取中传来的龙口数据改变采样步长的槽函数
void playback::receive_longkou_sampling_step_Slot()
{
    emit longkou_sampling_step_transmit(); //发送信号给主线程，更改主界面上的采样步长
}

//停止回放的槽函数
void playback::stopReviewSlot()
{
    playback_static_varying = 2; //更改此全局变量的值，以表示状态为停止回放
    emit start_playback(); //用以发送给主界面更新系统信息的信号
    emit zeroing_frame(); //用以发送给主界面将当前帧数和总帧数置为0
    global::playback_thread_stop = false; //关闭读取数据子线程中的循环，即关闭了子线程
    global::startreview_clicked = false; //关闭opengl中绘制的循环，即可停止主界面的刷新
}

//保存并退出的槽函数
void playback::exitSlot()
{
    playback_static_varying = 3; //更改此全局变量的值，以表示状态为保存并退出
    emit start_playback(); //用以发送给主界面更新系统信息的信号
    //将最后打开的文件名存入ini文件中
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    ini->setValue("/review_file/filename",ui->lineEdit->text());
    this->close(); //关闭界面
}

playback::~playback()
{
    delete ui;

}
