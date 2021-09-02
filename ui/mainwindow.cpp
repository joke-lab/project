#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtMath>
//#define pi 3.14;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "窗体高度" << this->height();
    qDebug() << "窗体宽度" << this->width();
    //this->setStyleSheet("QGroupBox, QPushButton, QLineEdit, QComboBox { background-color: #4682B4; color: #EEEEEE;}");
    //setMouseTracking(true);
    this->setWindowOpacity(1.0); //设置主窗口的透明度
    setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);//设置窗口最大化
    setWindowState(Qt::WindowMaximized);//窗口打开时为最大化状态
    QFont font2("Microsoft YaHei",12,75); //字体设置

    //设置状态栏中显示的内容（预热倒计时放入状态栏中）
    ui->statusbar->setMinimumHeight(30);//miniheight30

    lcd = new QLCDNumber; //显示雷达三分钟预热倒计时的显示屏
    lcd->resize(65,24);
    lcd->setSegmentStyle(QLCDNumber::Flat); //要对lcd的数字进行自定义样式时，需将其设置为Flat模式
    lcd->setVisible(false);

    preheat = new QLabel; // "雷达预热倒计时："字样
    preheat->resize(160,24);
    preheat->setText("雷达预热倒计时:");
    preheat->setStyleSheet("font-weight:bold;font-size:20px;color:red");
    preheat->setVisible(false);

    ui->statusbar->addWidget(preheat);
    ui->statusbar->addWidget(lcd);

    //状态栏显示鼠标坐标、旋转角度以及系统时间

    m_statusLabel = new QLabel;
    m_statusLabel->resize(200,24);
    ui->statusbar->addPermanentWidget(m_statusLabel);
    m_statusLabel->setVisible(true);

    currentTimeLabel = new QLabel; //获取当前时间
    currentTimeLabel->resize(200,24);
    QTimer *timer=new QTimer(this);
    timer->start(1000); // 每次发射timeout信号时间间隔为1秒
    ui->statusbar->addPermanentWidget(currentTimeLabel);
    currentTimeLabel->setVisible(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeUpdate()));

    //第一列区域显示
    QGroupBox *groupbox13 = new QGroupBox; //第一列“区域选择”的区域
    computationalAreaSel(groupbox13); //区域选择显示主界面
    QGroupBox *groupbox14 = new QGroupBox; //第一列“系统信息”的区域
    system_log(groupbox14); //系统设置显示主界面

    //OpenGL界面上的字符显示
    /*
    QWidget *gl2GLT = new QWidget; //gl界面上层左边布局
    QWidget *gl2GLB = new QWidget; //gl界面上层右边布局
    QWidget *gl2GRT = new QWidget; //gl界面上层左边布局
    QWidget *gl2GR = new QWidget; //gl界面上层左边布局
    QWidget *gl2GRB = new QWidget; //gl界面上层右边布局

    QGridLayout *gl2GLT = new QGridLayout; //gl界面上层左边布局
    QGridLayout *gl2GLB = new QGridLayout; //gl界面上层右边布局
    QGridLayout *gl2GRT = new QGridLayout; //gl界面上层左边布局
    QGridLayout *gl2GR = new QGridLayout; //gl界面上层左边布局
    QGridLayout *gl2GRB = new QGridLayout; //gl界面上层右边布局
    gl2GLB->setContentsMargins(0, 0, 0, 0);
    gl2GLB->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    //ui->gl2GLB->setContentsMargins(0, 0, 0, 0);
    //ui->gl2GLB->setAlignment(Qt::AlignLeft|Qt::AlignBottom);
    gl2GRT->setContentsMargins(0, 0, 0, 0);
    gl2GRT->setAlignment(Qt::AlignRight|Qt::AlignTop);
    //ui->gl2GR->setContentsMargins(0, 0, 0, 0);
    //ui->gl2GR->setAlignment(Qt::AlignRight); //颜色条
    ui->gl2GRB->setContentsMargins(0, 0, 0, 0);
    ui->gl2GRB->setAlignment(Qt::AlignRight|Qt::AlignBottom);
*/
    //ui->groupBox_rt->setAlignment(Qt::AlignRight|Qt::AlignBottom);
    //通讯状态，指示灯颜色
    ui->communication_status_value->setStyleSheet(m_red_SheetStyle); //雷达的通讯状态指示灯初始状态设置为红色
    ui->emission_status_value->setStyleSheet(m_red_SheetStyle); //发射机的通讯状态指示灯初始状态设置为红色
    ui->insStatus_v->setStyleSheet(m_red_SheetStyle); //惯导的通讯状态指示灯初始状态为红色

    //雷达预热选项
    ui->radio1->setChecked(true);
    ui->radio2->setChecked(false);

    //3-2部分-雷达信息
    /*QGroupBox *groupbox32 = new QGroupBox;
    groupbox32->setTitle("雷达信息");
    linethree_parttwo(groupbox32);*/
    glGLT(ui->gl2GLT);
    glGRT(ui->gl2GRT);
    glGLB(ui->gl2GLB);
    glGRB(ui->gl2GRB);
    //调用函数，添加色度条和强度值信息
    //glGR(gl2GR);

    //计算结果部分

    /*ui->tab31->verticalHeader()->setVisible(false);   //隐藏列表头
    ui->tab31->horizontalHeader()->setVisible(false); //隐藏行表头
    ui->tab31->setShowGrid(true); //网格线显示
    ui->tab31->setRowCount(12);
    ui->tab31->setColumnCount(2);
    //item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tab31->setSpan(0,0,1,2);
    ui->tab31->setSpan(1,0,2,2);
    ui->tab31->setSpan(3,0,1,1);
    ui->tab31->setSpan(3,1,1,1);
    ui->tab31->setSpan(4,0,2,1);
    ui->tab31->setSpan(4,1,2,1);
    ui->tab31->setSpan(6,0,1,1);
    ui->tab31->setSpan(6,1,1,1);
    ui->tab31->setSpan(7,0,2,1);
    ui->tab31->setSpan(7,1,2,1);
    ui->tab31->setSpan(9,0,1,1);
    ui->tab31->setSpan(9,1,1,1);
    ui->tab31->setSpan(10,0,2,1);
    ui->tab31->setSpan(10,1,2,1);
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setText("数据时间");
    item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item1->setFont(font2);
    item1V->setText("...");
    item1V->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item1V->setFont(font2);
    QTableWidgetItem *item21 = new QTableWidgetItem;
    item21->setText("有效波高（米）");
    item21->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item21->setFont(font2);
    item21V->setText("...");
    item21V->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item21V->setFont(font2);
    QTableWidgetItem *item22 = new QTableWidgetItem;
    item22->setText("峰波周期（秒）");
    item22->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item22->setFont(font2);
    item22V->setText("...");
    item22V->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item22V->setFont(font2);
    QTableWidgetItem *item31 = new QTableWidgetItem;
    item31->setText("峰波波向（度）");
    item31->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item31->setFont(font2);
    item31V->setText("...");
    item31V->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item31V->setFont(font2);
    QTableWidgetItem *item32 = new QTableWidgetItem;
    item32->setText("峰波波长（米）");
    item32->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item32->setFont(font2);
    item32V->setText("...");
    item32V->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item32V->setFont(font2);
    QTableWidgetItem *item41 = new QTableWidgetItem;
    item41->setText("流速（米）");
    item41->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item41->setFont(font2);
    item41V->setText("...");
    item41V->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item41V->setFont(font2);
    QTableWidgetItem *item42 = new QTableWidgetItem;
    item42->setText("流向（度）");
    item42->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item42->setFont(font2);
    item42V->setText("...");
    item42V->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item42V->setFont(font2);
    ui->tab31->setItem(0,0,item1);
    ui->tab31->setItem(1,0,item1V);
    ui->tab31->setItem(3,0,item21);
    ui->tab31->setItem(3,1,item22);
    ui->tab31->setItem(4,0,item21V);
    ui->tab31->setItem(4,1,item22V);
    ui->tab31->setItem(6,0,item31);
    ui->tab31->setItem(6,1,item32);
    ui->tab31->setItem(7,0,item31V);
    ui->tab31->setItem(7,1,item32V);
    ui->tab31->setItem(9,0,item41);
    ui->tab31->setItem(9,1,item42);
    ui->tab31->setItem(10,0,item41V);
    ui->tab31->setItem(10,1,item42V);
    qDebug() << "表格数据" << ui->tab31->rowCount() << ui->tab31->columnCount();
*/

    //绘制波图像的选项卡的控件配置及布局
    //创建一个选项卡控件，在每一页上嵌入QChartView作为画布，在每个画布上再嵌入QChart图表
    //chartview1->setBackgroundBrush(QBrush(QColor(14,10,50,255)));
    ui->tabwidget->addTab(chartview1,"海浪谱/波向谱");
    ui->tabwidget->setContentsMargins(0,0,0,0);
    //tabwidget->insertTab(0,chartview1,"海浪谱/波向谱");
    //tabwidget->insertTab(1,chartview3,"波数谱");

    chart1->addSeries(series1);
    chart1->addSeries(series2);
    chartview1->setChart(chart1);
    chartview1->setContentsMargins(0,0,0,0);

    //chart1->legend()->setBackgroundVisible(true);//设置背景是否可视
    //chart1->legend()->setAutoFillBackground(true);//设置背景自动填充
    chart1->setBackgroundVisible(false); //设置颜色

    series1->setName("海浪谱");
    series2->setName("波向谱");
    axis_x->setRange(0,4);
    axis_x->setTickCount(4);
    //axis_x->setMinorTickCount(5);
    axis_x->setLabelFormat("%0.1f");
    axis_y->setRange(0,1);
    axis_y->setTickCount(10);
    //axis_y->setMinorTickCount(5);
    axis_y->setLabelFormat("%0.1f");
    axis_y2->setRange(0,360);
    axis_y2->setTickCount(10);
    //axis_y2->setMinorTickCount(5);
    axis_y2->setLabelFormat("%0.0f"); //坐标轴角度单位保留整数

    chart1->addAxis(axis_x,Qt::AlignBottom);
    chart1->addAxis(axis_y,Qt::AlignLeft);
    chart1->addAxis(axis_y2,Qt::AlignRight);
    series1->attachAxis(axis_x);
    series1->attachAxis(axis_y);
    series1->setColor(QColor(255,0,0));
    series2->attachAxis(axis_x);
    series2->attachAxis(axis_y2);
    series2->setColor(QColor(0,0,255));
    //ui->tab31->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //计算结果部分横向填满布局
    //ui->tab31->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch); //计算结果部分纵向填满布局
    //ui->vlayT->setMargin(0);
    //ui->vlayT->addWidget(tab31,5);
    //ui->vlayT->addWidget(tabwidget,5);

    //编辑菜单栏“设置”的下拉菜单   开始处理变成停止处理存在问题
    /*if(global::start_or_stop_dealing)
    {
        ui->action11->setText("开始处理");
    }
    else
    {
        ui->action11->setText("停止处理");
    }*/

    //多线程的实例化和启动部分
    share_buffer = new uchar[4040*20*N]; //共享内存(10个角度为一组)
    angleArray = new float[50*N]; //用于雷达数据采集中的角度值内存
    share_lock = new QReadWriteLock(QReadWriteLock::NonRecursive); //用于全局的读写锁,无递归
    ins_buffer = new double[160*N+1]; //惯导的共享内存，一个计数位，5组惯导数据
    mutex = new QMutex;
    figurex_buffer = new double[3600];
    figurez_buffer = new double[3600*2001]; //计算的共享内存
    figure_mutex = new QMutex;
    save_buffer = new uchar[10*N*2000]; //存储的共享内存
    saveAngleBuffer = new float[10*N];
    save_mutex = new QReadWriteLock;

    //startprocessing线程（采集）
    packet_data_thread = new QThread;
    start_process = new startprocessing(share_buffer,angleArray,share_lock,nullptr); //实时处理中的开始处理类
    start_process->moveToThread(packet_data_thread);
    connect(packet_data_thread,&QThread::finished,packet_data_thread,&QObject::deleteLater);
    connect(packet_data_thread,&QThread::finished,start_process,&QObject::deleteLater);
    connect(this,&MainWindow::start_packet_data_thread,start_process,&startprocessing::packet_grab_thread); //将数据抓包的信号与数据抓包类中的函数对应
    connect(start_process,&startprocessing::start_process_message,this,&MainWindow::start_process_receiveMessageSlot); //将数据抓包线程中的信号与主线程中消息接收的槽函数相连接
    packet_data_thread->start(); //开启四个子线程

    //startsavingtoharddisk线程（将数据存入硬盘）
    harddisk_thread = new QThread;
    start_save_harddisk = new startsavingtoharddisk(save_buffer,saveAngleBuffer,save_mutex,nullptr); //实时处理中将数据存入硬盘的类
    start_save_harddisk->moveToThread(harddisk_thread);
    connect(harddisk_thread,&QThread::finished,harddisk_thread,&QObject::deleteLater);
    connect(harddisk_thread,&QThread::finished,start_save_harddisk,&QObject::deleteLater);
    connect(this,&MainWindow::start_harddisk_thread,start_save_harddisk,&startsavingtoharddisk::harddisk_save_thread); //将存入硬盘的信号与存入硬盘类中的函数对应
    connect(start_save_harddisk,&startsavingtoharddisk::start_saving_message,this,&MainWindow::start_saving_receiveMessageSlot); //将存入硬盘线程中的信号与主线程中消息接收的槽函数相连接
    harddisk_thread->start();

    //paintonline线程（数据预处理）
    paint_online_thread = new QThread;
    paint_online = new paintonline(share_buffer,angleArray,share_lock,ins_buffer,mutex,figurex_buffer,figurez_buffer,figure_mutex,save_buffer,saveAngleBuffer,save_mutex,nullptr); //实时处理中计算数据的类
    paint_online->moveToThread(paint_online_thread);
    connect(paint_online_thread,&QThread::finished,paint_online_thread,&QObject::deleteLater);
    connect(paint_online_thread,&QThread::finished,paint_online,&QObject::deleteLater);
    connect(this,&MainWindow::paint_online_signal_thread,paint_online,&paintonline::paintonline_thread); //将实时画图的信号与实时画图类中的函数对应
    connect(paint_online,&paintonline::paint_online_message,this,&MainWindow::paint_online_receiveMessageSlot); //将实时画图线程中的信号与主线程中接收的槽函数相连接
    paint_online_thread->start();

    //insData线程（惯导数据读取）
    ins_data_thread = new QThread;
    ins_Data = new insdata(ins_buffer,mutex,nullptr);
    ins_Data->moveToThread(ins_data_thread);
    connect(ins_data_thread,&QThread::finished,ins_data_thread,&QObject::deleteLater);
    connect(ins_data_thread,&QThread::finished,ins_Data,&QObject::deleteLater);
    connect(this,&MainWindow::start_ins_thread,ins_Data,&insdata::insData_grab_thread2); //调用惯导数据读取线程中的函数
    ins_data_thread->start();

    //figure线程（计算）
    figure_thread = new QThread;
    start_figure = new figure(figurex_buffer,figurez_buffer,figure_mutex,nullptr); //实时计算的类
    start_figure->moveToThread(figure_thread);
    connect(figure_thread,&QThread::finished,figure_thread,&QObject::deleteLater); //结束时，所有线程自杀以防止溢出
    connect(figure_thread,&QThread::finished,start_figure,&QObject::deleteLater);
    connect(this,&MainWindow::figure_initial_thread,start_figure,&figure::figure_thread1); //将实时部分开始计算的信号于计算线程中的函数对应起来
    connect(paint_online,SIGNAL(figurethread(double *)),start_figure,SLOT(figure_thread2(double *)),Qt::DirectConnection); //paintonline线程调用计算线程中的函数
    connect(start_figure,&figure::databaseError,this,&MainWindow::databaseError_Slot);
    figure_thread->start();

    connect(this,&MainWindow::update_openglwidget,ui->widget,&openglwidget::area_choose); //发送信号调用opengl界面上的刷新函数，使其更新
    //QObject::connect(paint_online,&paintonline::chartvalue,this,&MainWindow::receive_longkou_figureresult_Slot,Qt::DirectConnection);

    //读取数据的子线程
    playback_readfile_thread = new QThread; //将读取数据的子线程实例化
    pb_data = new playback_readdata; //回放部分处理数据的类
    pb_data->moveToThread(playback_readfile_thread);
    connect(playback_readfile_thread,&QThread::finished,playback_readfile_thread,&QObject::deleteLater);
    connect(playback_readfile_thread,&QThread::finished,pb_data,&QObject::deleteLater);
    playback_readfile_thread->start();

    //波形显示的子线程
    wavedisplay_thread = new QThread;
    wavedisplay = new waveview; //波形显示的类
    wavedisplay->moveToThread(wavedisplay_thread);
    connect(wavedisplay_thread,&QThread::finished,wavedisplay_thread,&QObject::deleteLater);
    connect(wavedisplay_thread,&QThread::finished,wavedisplay,&QObject::deleteLater);
    wavedisplay_thread->start();

    QObject::connect(pb_data,&playback_readdata::start_showing,play_back,&playback::start_showing_Slot); //当读取线程中第一次进行了内存交换后调用槽函数
    QObject::connect(pb_data,SIGNAL(pb_paint(qint16*)),ui->widget,SLOT(receive_pb_paint(qint16*)),Qt::DirectConnection);
    QObject::connect(pb_data,SIGNAL(rraw_paint(int,qint16*)),ui->widget,SLOT(receive_rraw_paint(int,qint16*)),Qt::DirectConnection); //RRAW格式的回放
    QObject::connect(pb_data,SIGNAL(pt_paint(int,int,int,int,qint16*)),ui->widget,SLOT(receive_pt_paint(int,int,int,int,qint16*)),Qt::DirectConnection);
    QObject::connect(pb_data,SIGNAL(rrawInsShow(double,double,double,double)),this,SLOT(receive_rrawInsShow(double,double,double,double)),Qt::DirectConnection);
    QObject::connect(play_back,&playback::start_playback_readfile_thread,pb_data,&playback_readdata::startread_thread); //将数据开始读取的信号与子线程中的耗时函数连接
    QObject::connect(pb_data,&playback_readdata::pingtan_change_distance,this,&MainWindow::pingtan_distance_change); //从回放计算子线程传回的平潭数据改变观测范围的槽函数
    QObject::connect(pb_data,&playback_readdata::longkou_sampling_step,this,&MainWindow::receive_longkou_sampling_step_Slot); //接收来自数据回放线程改变主界面上显示“采样步长”的槽函数
    QObject::connect(play_back,&playback::start_playback,this,&MainWindow::receive_start_playback_Slot); //接收从开始回放界面传来的改变系统信息的槽函数
    QObject::connect(ui->widget,SIGNAL(change_current_orientation(float)),this,SLOT(receive_change_current_orientation_Slot(float))); //接收来自opengl界面显示时传来的实时改变主界面上的“当前方位”值的槽函数
    QObject::connect(ui->widget,SIGNAL(change_current_frame()),this,SLOT(receive_current_frame_Slot())); //接收来自opengl界面与读取数据的子线程所用的共享内存交换时传来的信号，用以更新“当前帧数”的槽函数
    QObject::connect(play_back,&playback::zeroing_frame,this,&MainWindow::receive_current_frame_clear_Slot); //从playback的停止回放的槽函数中接收到信号，用以将主界面的当前帧数和总帧数置为0
    QObject::connect(pb_data,SIGNAL(playback_figure_returnvalue(QString,double,double,double,double,double,double,double,double *,double *,double *,double *)),this,SLOT(receive_longkou_figureresult_Slot(QString,double,double,double,double,double,double,double,double *,double *,double *,double *)),Qt::DirectConnection); //用于接收来自playback界面的信号，当龙口数据回放完毕后，将计算结果值显示在主界面上

    //新增功能（波形显示）
    QObject::connect(paint_online,SIGNAL(st_wave(int, int, qint16 *)),wavedisplay,SLOT(openSlot(int, int, qint16 *)),Qt::DirectConnection); //实时发送至波形显示
    QObject::connect(pb_data,SIGNAL(st_ws(int, int, qint16 *)),wavedisplay,SLOT(openSlot(int, int, qint16 *))); //回放

    //信号与槽的连接
    QObject::connect(ui->action21,SIGNAL(triggered(bool)),this,SLOT(open_radar_power_Slot())); //雷达控制菜单-打开雷达
    QObject::connect(ui->action22,SIGNAL(triggered(bool)),this,SLOT(open_radar_transceiver_Slot())); //雷达控制菜单-打开发射机
    QObject::connect(ui->action32,SIGNAL(triggered(bool)),this,SLOT(regionSelectionSlot())); //数据处理菜单-区域选择
    QObject::connect(batch,&batchproc::open_regionselection,this,&MainWindow::regionSelectionSlot); //批处理中打开区域选择
    QObject::connect(ui->action33,SIGNAL(triggered(bool)),this,SLOT(figureResultSlot())); //数据处理菜单-计算结果
    QObject::connect(ui->action31,SIGNAL(triggered(bool)),this,SLOT(batchProcSlot())); //数据处理菜单-批处理
    QObject::connect(ui->action34,SIGNAL(triggered(bool)),this,SLOT(waveformSlot())); //数据处理菜单-波形显示
    QObject::connect(ui->action41,SIGNAL(triggered(bool)),this,SLOT(runningSetSlot())); //设置菜单-运行设置
    QObject::connect(ui->action42,SIGNAL(triggered(bool)),this,SLOT(ColorSetSlot())); //设置菜单-颜色设置
    QObject::connect(ui->action43,SIGNAL(triggered(bool)),this,SLOT(systemsetSlot())); //设置菜单-系统设置
    QObject::connect(batch,&batchproc::open_system,this,&MainWindow::systemsetSlot); //批处理中打开系统设置
    QObject::connect(ui->action44,SIGNAL(triggered(bool)),this,SLOT(comsetSlot())); //设置菜单-串口设置
    QObject::connect(ui->action11,SIGNAL(triggered(bool)),this,SLOT(startprocessingSlot())); //系统菜单-开始处理
    QObject::connect(ui->action12,SIGNAL(triggered(bool)),this,SLOT(dataReviewSlot())); //系统菜单-数据回放
    QObject::connect(ui->action13,SIGNAL(triggered(bool)),this,SLOT(instestSlot())); //系统菜单-惯导测试
    QObject::connect(ui->action51,SIGNAL(triggered(bool)),this,SLOT(helpSlot())); //帮助菜单-帮助
    QObject::connect(ui->action52,SIGNAL(triggered(bool)),this,SLOT(relatedSlot())); //帮助菜单-关于
    QObject::connect(ui->radioButton1,SIGNAL(clicked(bool)),this,SLOT(area_Slot())); //区域显示按钮的槽函数
    QObject::connect(ui->radioButton2,SIGNAL(clicked(bool)),this,SLOT(area_Slot())); //区域隐藏按钮的槽函数

    //“打开雷达”和“打开收发机”对应的信号与槽的连接
    QObject::connect(ui->power_radar,SIGNAL(clicked(bool)),this,SLOT(open_radar_power_Slot())); //“打开雷达”图片按钮对应的槽函数
    QObject::connect(ui->transceiver_turnon,SIGNAL(clicked(bool)),this,SLOT(open_radar_transceiver_Slot())); //“打开收发机”图片按钮对应的槽函数

    //各个界面保存时，对应的信号与槽的连接
    QObject::connect(color_Set,&color_set::colorbar_change,this,&MainWindow::color_set_change_Slot); //当颜色设置界面保存时，发送信号调用此函数来改变主界面上的可观测强度值条
    QObject::connect(run_set,&running_set::runset_send_to_loginformation,this,&MainWindow::receive_runset_to_loginformation_Slot); ////当运行设置保存时用以更新系统信息的槽函数
    QObject::connect(regionselection,&region_selection::area_change_Slot,this,&MainWindow::calculate_area_change_Slot); //当区域选择界面保存时，发送信号调用此函数用以改变主界面上的可观测范围的值
    QObject::connect(system_Set,&system_set::update_radarinform,this,&MainWindow::update_radarinform_Slot); //系统设置界面关闭时，发送信号调用此函数以在主界面上显示最新的采样步长和起始结束距离

    QObject::connect(play_back,&playback::pingtan_change_distance_second,this,&MainWindow::pingtan_distance_change); //从回放界面传来的平潭数据改变观测范围的槽函数
    QObject::connect(play_back,&playback::start_playback,this,&MainWindow::receive_start_playback_Slot); //接收从开始回放界面传来的改变系统信息的槽函数
    QObject::connect(ui->widget,SIGNAL(change_current_orientation(float)),this,SLOT(receive_change_current_orientation_Slot(float))); //接收来自opengl界面显示时传来的实时改变主界面上的“当前方位”值的槽函数
    QObject::connect(play_back,&playback::longkou_sampling_step_transmit,this,&MainWindow::receive_longkou_sampling_step_Slot); //接收来自数据回放线程改变主界面上显示“采样步长”的槽函数
    QObject::connect(ui->widget,SIGNAL(change_current_frame()),this,SLOT(receive_current_frame_Slot())); //接收来自opengl界面与读取数据的子线程所用的共享内存交换时传来的信号，用以更新“当前帧数”的槽函数
    QObject::connect(play_back,&playback::zeroing_frame,this,&MainWindow::receive_current_frame_clear_Slot); //从playback的停止回放的槽函数中接收到信号，用以将主界面的当前帧数和总帧数置为0
    //QObject::connect(play_back,&playback::playback_longkou_figure,this,&MainWindow::receive_longkou_figureresult_Slot); //用于接收来自playback界面的信号，当龙口数据回放完毕后，将计算结果值显示在主界面上

    QObject::connect(ins_Data,&insdata::insinitialEnd,this,&MainWindow::start_otherthread_Slot); //接收来自惯导的线程，开始进行实时处理
    QObject::connect(this,&MainWindow::insinitial_thread,ins_Data,&insdata::insData_grab_thread1); //开始处理时，首先调用惯导线程的初始化函数
    QObject::connect(paint_online,SIGNAL(frame(int)),this,SLOT(receive_online_frame_Slot(int)),Qt::DirectConnection);
    QObject::connect(start_figure,SIGNAL(result(QString,double,double,double,double,double*,double*,double*,double*)),this,SLOT(receive_online_result_Slot(QString,double,double,double,double,double*,double*,double*,double*)),Qt::DirectConnection);
    QObject::connect(paint_online,SIGNAL(test_19252008(qint16*,int)),ui->widget,SLOT(receive_test_12252008(qint16*,int)),Qt::DirectConnection); //接收从paintonline线程传来的信号，发送至openglwidget中，传递更改的数组信息
    QObject::connect(start_process,SIGNAL(preprocessing(int)),paint_online,SLOT(paintonline_thread2(int)));
    //QObject::connect(paint_online,SIGNAL(gps_toheadSave(int *)),start_save_harddisk,SLOT(gps_toheadSave_Slot(int *)),Qt::BlockingQueuedConnection); //从paintonline中发送经纬度到存储数据的线程，用于数据头中经纬度信息的存储
    //QObject::connect(paint_online,&paintonline::save_harddisk_thread,start_save_harddisk,&startSavingtoharddisk::harddisk_save_thread2,Qt::BlockingQueuedConnection);
    QObject::connect(paint_online,SIGNAL(save_harddisk_thread(double *)),start_save_harddisk,SLOT(harddisk_save_thread2(double *)),Qt::BlockingQueuedConnection);
    //QObject::connect(start_process,&startProcessing::preprocessing,start_save_harddisk,&startSavingtoharddisk::harddisk_save_thread2,Qt::BlockingQueuedConnection);
    QObject::connect(paint_online,SIGNAL(naviValue(int,int,double,double)),this,SLOT(onlinenavi_show(int,int,double,double))); //接收来自paintonline的实时部分的惯导数据，并显示在主界面上
    QObject::connect(batch,SIGNAL(batch_result(double,double,double,double)),this,SLOT(receive_batch_result(double,double,double,double)),Qt::DirectConnection);

}

//状态栏初始函数
void MainWindow::initStatusBar()
{

}

MainWindow::~MainWindow()
{
    figure_thread->quit();
    figure_thread->wait();
    packet_data_thread->quit();
    packet_data_thread->wait();
    harddisk_thread->quit();
    harddisk_thread->wait();
    paint_online_thread->quit();
    paint_online_thread->wait();
    figure_thread->quit();
    figure_thread->wait();
    delete []share_buffer;
    delete ui;

}


//鼠标移动识别方位角，距离以及强度值
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    ui->widget->setMouseTracking(true);
    //x,y为左上角为坐标原点获取鼠标位置
    //setMouseTracking(true);
    float x1 = ui->widget->pos().x(); //widget的左上角坐标
    float y1 = ui->widget->frameGeometry().y(); //widget的左上角坐标(未添加菜单栏等高度)
    int h1 = ui->toolBar->height();
    int h2 = ui->menubar->height();
    int height = ui->widget->height();
    int width = ui->widget->width();//widget窗口的宽、高
    float ax = x1+width/2;
    float ay = y1+height/2+h1+h2; //将OpenGL界面中心点作为零点
    //ui->widget->move(x1+width/2,y1+height/2);
    float x = event->pos().x(); //鼠标实时的x坐标
    float y = event->pos().y(); //鼠标实时的y坐标
    double tempx = x-ax;
    double tempy = y-ay; //角度的x,y
    float tempangle = qAtan2(tempy,tempx); //qreal默认类型为double
    if(tempangle>=-M_PI_2&&tempangle<=M_PI)
        tempangle = tempangle + M_PI_2;
    else
        tempangle =tempangle + 5*M_PI/2;
    float angle = tempangle*180/M_PI;
    qDebug() << "widget坐标" << x1 << y1 << height << width <<y;
    QString str = "(" + QString::number(angle,'f',1)+"°"+" "+ "," + QString::number(y)+"m"+ ")";
    m_statusLabel->setText(str);
}

//状态栏时间
void MainWindow::timeUpdate()

{
    QDateTime currentTime = QDateTime::currentDateTime(); //获取现在的时间
    QString Timestr = currentTime.toString("yyyy/MM/dd hh:mm:ss");
    currentTimeLabel->setText(Timestr);
}

//“计算区域选择”部分的界面布局函数
void MainWindow::computationalAreaSel(QGroupBox *groupbox13)
{
    QGroupBox *box = new QGroupBox;
    box = groupbox13;
    radioButtonGroup->addButton(ui->radioButton1,0);
    radioButtonGroup->addButton(ui->radioButton2,1);
    ui->radioButton1->setChecked(true); //设置选中初始化
    global::area_display = true; //将全局变量中的区域显示变量设置为1

    //打开ini文件
    QSettings *region_selection_ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    qDebug() << "sasdfghj" <<  QCoreApplication::applicationDirPath()+"/setting.ini";
    ui->angle_min->setText(region_selection_ini->value("/area/angle1").toString());
    ui->angle_max->setText(region_selection_ini->value("/area/angle2").toString());
    ui->radio_min->setText(region_selection_ini->value("/area/radio1").toString());
    ui->radio_max->setText(region_selection_ini->value("/area/radio2").toString());
}

void MainWindow::glGLT(QGridLayout *gl2GLT)
{
    //QGridLayout *lay = new QGridLayout;
    //lay = gl2GLT;
    QFont f("Microsoft YaHei",9,75);
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString input_mini = ini->value("/radar_information/minimum").toString(); //读取显示观测范围的最小值
    QString input_maxi = ini->value("/radar_information/maximum").toString(); //读取显示观测范围的最大值
    /*QLabel *minidistance = new QLabel;
    minidistance->setText("起始距离:");
    minidistance->setFont(f);
    QLabel *maxidistance = new QLabel;
    maxidistance->setText("结束距离:");
    maxidistance->setFont(f);*/
    ui->minidistance_v->setText(input_mini);
    ui->minidistance_v->setFont(f);
    ui->maxidistance_v->setText(input_maxi);
    ui->maxidistance_v->setFont(f);
    /*QLabel *sampling_step_size = new QLabel;
    sampling_step_size->setText("采样步长:");
    sampling_step_size->setFont(f);*/
    QString step = ini->value("/radar_information/samplestep").toString(); //读取初始的采样步长
    ui->sampling_step_size_value->setText(step);
    ui->sampling_step_size_value->setFont(f);
    QLabel *trigger_delay = new QLabel;
    trigger_delay->setText("触发延时:");
    trigger_delay->setFont(f);
    QString tri = ini->value("/radar_information/triggerdelay").toString(); //读取初始的采样步长
    ui->trigger_delay_v->setText(tri);
    ui->trigger_delay_v->setFont(f);
    QLabel *base_noise = new QLabel;
    base_noise->setText("噪声基底参数:");
    base_noise->setFont(f);
    QString noise = ini->value("/radar_information/noisebasevalue").toString(); //读取初始的采样步长
    ui->base_noise_v->setText(noise);
    ui->base_noise_v->setFont(f);

}

void MainWindow::glGRT(QGridLayout *gl2GRT)
{
    QGridLayout *lay = new QGridLayout;
    lay = gl2GRT;
    QFont f("Microsoft YaHei",9,75);
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QLabel *current_frame = new QLabel;
    current_frame->setText("当前帧数：");
    current_frame->setFont(f);
    ui->current_frame_value->setText("...");
    ui->current_frame_value->setFont(f);
    QLabel *total_frame = new QLabel;
    total_frame->setText("总帧数:");
    total_frame->setFont(f);
    ui->total_frame_value->setText("...");
    ui->total_frame_value->setFont(f);
    QLabel *acquisition_mode = new QLabel;
    acquisition_mode->setText("当前采集方式:");
    acquisition_mode->setFont(f);
    int id = ini->value("/collection_mode/Id").toInt();
    if(id == 0)
        ui->acquisition_mode_v->setText("手动采集");
    else if(id == 1)
        ui->acquisition_mode_v->setText("每小时平均采集");
    else if(id == 2)
        ui->acquisition_mode_v->setText("累计平均采集");
    ui->acquisition_mode_v->setFont(f);
    QLabel *site_type = new QLabel;
    site_type->setText("站点类型:");
    site_type->setFont(f);
    int site1 = ini->value("/site_information/onBoard").toInt();
    int site2 = ini->value("/site_information/shoreBased").toInt();
    if(site1 == 1)
        ui->site_type_v->setText("船载");
    else if(site2 == 1)
        ui->site_type_v->setText("岸基");
    ui->site_type_v->setFont(f);

}

void MainWindow::glGLB(QGridLayout *gl2GLB)
{
    QGridLayout *lay = new QGridLayout;
    lay = gl2GLB;
    QFont f("Microsoft YaHei",9,75);
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QLabel *tdomain_points = new QLabel;
    tdomain_points->setText("时域点数：");
    tdomain_points->setFont(f);
    QString tdomain = ini->value("/calculation_parameter/time_domain_points").toString();
    ui->tdomain_points_v->setText(tdomain);
    ui->tdomain_points_v->setFont(f);
    QLabel *airx_points = new QLabel;
    airx_points->setText("Nx:"); //空域点数Nx
    airx_points->setFont(f);
    QString xdomain = ini->value("/calculation_parameter/airspace_points_x").toString();
    ui->airx_points_v->setText(xdomain);
    ui->airx_points_v->setFont(f);
    QLabel *airy_points = new QLabel; //Ny
    airy_points->setText("Ny:"); //空域点数Ny
    airy_points->setFont(f);
    QString ydomain = ini->value("/calculation_parameter/airspace_points_y").toString();
    ui->airy_points_v->setText(ydomain);
    ui->airy_points_v->setFont(f);
    //QLabel *correct_angle = new QLabel;
    //correct_angle->setText("计算修正角度（度）:");
    //correct_angle->setFont(f);
    //QString correcta = ini->value("/calculation_parameter/radar_correction_angle").toString();
    //correct_angle_v->setText(correcta);
    //correct_angle_v->setFont(f);
    QLabel *water_deep = new QLabel;
    water_deep->setText("水深（米）:");
    water_deep->setFont(f);
    QString deep = ini->value("/calculation_parameter/water_depth").toString();
    ui->water_deep_v->setText(deep);
    ui->water_deep_v->setFont(f);

}

void MainWindow::glGRB(QGridLayout *gl2GRB)
{
    QGridLayout *lay = new QGridLayout;
    lay = gl2GRB;
    QFont f("Microsoft YaHei",9,75);
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QLabel *save_frame = new QLabel;
    save_frame->setText("存储帧数:");
    save_frame->setFont(f);
    QString save = ini->value("/collection_mode/framenumber").toString();
    ui->save_frame_v->setText(save);
    ui->save_frame_v->setFont(f);
    QLabel *dertat = new QLabel;
    dertat->setText("时间采样间隔:");
    dertat->setFont(f);
    save = ini->value("/calculation_parameter/sampling_interval_time").toString();
    ui->dertat_v->setText(save);
    ui->dertat_v->setFont(f);
    QLabel *dertax = new QLabel;
    dertax->setText("x抽样间隔:");
    dertax->setFont(f);
    save = ini->value("/calculation_parameter/sampling_interval_x").toString();
    ui->dertax_v->setText(save);
    ui->dertax_v->setFont(f);
    QLabel *dertay = new QLabel;
    dertay->setText("y抽样间隔:");
    dertay->setFont(f);
    save = ini->value("/calculation_parameter/sampling_interval_y").toString();
    ui->dertay_v->setText(save);
    ui->dertay_v->setFont(f);

}

//“打开雷达”图片按钮和“打开雷达”下拉单对应的槽函数
//当当前雷达状态为打开时，power_radar_bool为正，当雷达状态为关闭时，power_radar_bool为负
void MainWindow::open_radar_power_Slot()
{
    QDateTime time = QDateTime::currentDateTime(); //获取当前的系统时间
    if(global::power_radar_bool) //当power_radar_bool为正时，说明此时雷达处于打开状态，此次点击是为了关闭雷达
    {
        //当发射机处于打开状态时，不能关闭雷达
        /*if(transceiver_turnon_bool)
        {
            QMessageBox::warning(this,"发射机已打开","请先关闭发射机，再关闭雷达",QMessageBox::Ok);
            return;
        }*/
        poweroff_Slot(); //调用关闭雷达的功能函数
        if(!global::power_radar_bool) //当雷达成功关闭时
        {
            radaroff_status_Slot(); //改变雷达通讯状态的指示灯
            ui->action21->setText("打开雷达");
            ui->power_radar->setStyleSheet("QPushButton{border-image: url("+QCoreApplication::applicationDirPath()+"/icon/power_on.png)}");
            ui->power_radar_label->setText("打开雷达");
            ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"关闭雷达");
        }
    }
    else //取反后为负，则表明点击后应为“关闭雷达”的图片
    {
        poweron_Slot(); //此时调用打开雷达的功能函数
        if(global::power_radar_bool) //当雷达成功打开时
        {
            radaron_status_Slot(); //改变雷达通讯状态的指示灯
            ui->action21->setText("关闭雷达");
            ui->power_radar->setStyleSheet("QPushButton{border-image: url("+QCoreApplication::applicationDirPath()+"/icon/power_off.png)}");
            ui->power_radar_label->setText("关闭雷达");
            ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"打开雷达");
            if(ui->radio1->isChecked())
            {
                lcd->setFrameStyle(QFrame::NoFrame);
                QPalette lcdp = lcd->palette();
                lcdp.setColor(QPalette::Foreground,QColor(qRgb(255,0,0))); //设置Lcd的颜色
                lcd->setPalette(lcdp);
                int minu = 3;
                int sec = 34;
                QString str = "03:35"; //初始显示值为三分半钟
                lcd->display(str);
                preheat->setVisible(true);
                lcd->setVisible(true);
                preheating = true; //在三分钟倒计时中，将此变量设为真，用以其他操作对此状态进行判断
                for(int i=0;i<215;i++)
                {
                    QElapsedTimer t;
                    t.start(); //一秒钟计时开始
                    if(sec==0) //当秒为0时
                    {
                        str = QString("0%1:0%2").arg(minu).arg(sec);
                        minu--;
                        sec = 59;
                    }
                    else if(sec!=0 && sec<10) //当秒数为一位数时
                    {
                        str = QString("0%1:0%2").arg(minu).arg(sec);
                        sec--;
                    }
                    else
                    {
                        str = QString("0%1:%2").arg(minu).arg(sec);
                        sec--;
                    }
                    while(t.elapsed() < 1000)
                        QCoreApplication::processEvents();
                    lcd->display(str);
                }
                preheat->setVisible(false); //三分钟倒计时结束后，隐藏状态栏中的倒计时部分
                lcd->setVisible(false);
            }
            preheating = false; //三分钟倒计时结束
        }
    }
}

//“打开雷达”的功能槽函数
void MainWindow::poweron_Slot()
{
    pcap_t *fp;
    char errbuf[PCAP_ERRBUF_SIZE];
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString name_all = ini->value("/NICname_choosen/nicname").toString(); //从初始文件中获取的网卡名称
    QStringList list = name_all.split("}"); //由于初始文件中保存的网卡名称包含了网卡名称和网卡描述两个部分，因此需要分开，并将网卡名称部分提取出来
    QString nicname = list.at(0)+"}"; //以}为分界点，划分时}符号归为后一部分
    QByteArray name_bytearray = nicname.toLatin1(); //先将字符串类型转换为二进制流的形式，不可一次性转为char，转换不成功
    char *nic = name_bytearray.data(); //将网卡名称转为char类型，便于下面使用

    /* 打开输出设备 */
    if ( (fp= pcap_open_live(nic,1,1,1000,errbuf))==nullptr)
    {
        QMessageBox::warning(nullptr,"网口不可用","网口打开失败，请检查相关网口设置，确保网卡选择正确。",QMessageBox::Ok,0);
        return;
    }
    //c4 09
    u_char packet[29] = {0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x01,0x02,0x03,0x04,0x05,0x06,0xa5,0x00,0x01,0x07,0x00,0x00,0x00,0x00,0xb8,0x0b,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    //关闭雷达的命令
    //u_char packet[29] = {0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x01,0x02,0x03,0x04,0x05,0x06,0xa5,0x00,0x00,0x07,0xa0,0x00,0x00,0x00,0x18,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    /* 发送数据包 */
    if (pcap_sendpacket(fp, packet, 29) != 0)
    {
        QMessageBox::warning(nullptr,"雷达打开失败","雷达打开失败，请检查相关硬件连接及网口设置后重新尝试。",QMessageBox::Ok,0);
        //fprintf(stderr,"/nError sending the packet: /n", pcap_geterr(fp));
        return;
    }
    qDebug() << "Mainwindow中调用打开雷达的函数";
    global::power_radar_bool = !global::power_radar_bool; //此时雷达打开成功，更改雷达状态变量
}

//“关闭雷达”的功能槽函数
void MainWindow::poweroff_Slot()
{
    pcap_t *fp;
    char errbuf[PCAP_ERRBUF_SIZE];
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString name_all = ini->value("/NICname_choosen/nicname").toString(); //从初始文件中获取的网卡名称
    QStringList list = name_all.split("}"); //由于初始文件中保存的网卡名称包含了网卡名称和网卡描述两个部分，因此需要分开，并将网卡名称部分提取出来
    QString nicname = list.at(0)+"}"; //以}为分界点，划分时}符号归为后一部分
    QByteArray name_bytearray = nicname.toLatin1(); //先将字符串类型转换为二进制流的形式，不可一次性转为char，转换不成功
    char *nic = name_bytearray.data(); //将网卡名称转为char类型，便于下面使用

    /* 打开输出设备 */
    if ( (fp= pcap_open_live(nic,1,1,1000,errbuf))==nullptr)
    {
        QMessageBox::warning(nullptr,"网口不可用","网口打开失败，请检查相关网口设置，确保网卡选择正确。",QMessageBox::Ok,0);
        return;
    }
    u_char packet[29] = {0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x01,0x02,0x03,0x04,0x05,0x06,0xa5,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0xb8,0x0b,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    /* 发送数据包 */
    if (pcap_sendpacket(fp, packet, 29 /* size */) != 0)
    {
        QMessageBox::warning(nullptr,"雷达关闭失败","雷达关闭失败，请检查相关硬件连接及网口设置后重新尝试。",QMessageBox::Ok,0);
        return;
    }
    global::power_radar_bool = !global::power_radar_bool; //此时雷达关闭成功，更改雷达状态变量
    preheat->setVisible(false); //隐藏雷达预热倒计时字样
    lcd->setVisible(false); //隐藏三分钟倒计时数字
}

//雷达打开时状态指示灯改变的功能槽函数
void MainWindow::radaron_status_Slot()
{
    ui->communication_status_value->setStyleSheet(m_green_SheetStyle);
}

//雷达关闭时状态指示灯改变的功能槽函数
void MainWindow::radaroff_status_Slot()
{
    ui->communication_status_value->setStyleSheet(m_red_SheetStyle);
}

//“打开收发机”图片按钮和“打开收发机”下拉单对应的槽函数
//当当前发射机状态为打开时，transceiver_turnon_bool为正，当发射机状态为关闭时，transceiver_turnon_bool为负
void MainWindow::open_radar_transceiver_Slot()
{
    QDateTime time = QDateTime::currentDateTime(); //获取现在的时间
    if(global::transceiver_turnon_bool) //当transceiver_turnon_bool为正时，说明此时发射机处于打开状态，此次点击是为了关闭发射机
    {
        transceiver_Slot(); //调用关闭发射机的功能函数
        if(!global::transceiver_turnon_bool) //当发射机成功关闭时
        {
            transmitoff_status_Slot(); //改变发射机通讯状态的指示灯
            ui->action22->setText("打开发射机");
            ui->transceiver_turnon->setStyleSheet("QPushButton{border-image: url("+QCoreApplication::applicationDirPath()+"/icon/trans_on.png)}");
            ui->transceiver_turnon_label->setText("打开发射机");
            ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"关闭发射机");
        }
    }
    else //当transceiver_turnon_bool为负时，说明此时发射机处于关闭状态，此次点击是为了打开发射机
    {
        //当雷达处于未打开状态时，不能打开发射机
        if(!global::power_radar_bool)
        {
            QMessageBox::warning(this,"雷达未打开","请先打开雷达，等待预热三分钟后再打开发射机",QMessageBox::Ok);
            return;
        }
        //当雷达处于预热倒计时三分钟时，不能打开发射机
        if(preheating)
        {
            QMessageBox::warning(this,"雷达预热中","雷达预热中，请等三分钟倒计时结束后再打开发射机",QMessageBox::Ok);
            return;
        }
        qDebug() << "此处调用";
        transceiver_Slot(); //调用打开发射机的功能函数
        if(global::transceiver_turnon_bool) //当发射机打开成功时
        {
            transmiton_status_Slot(); //改变发射机通讯状态的指示灯
            ui->action22->setText("关闭发射机");
            ui->transceiver_turnon->setStyleSheet("QPushButton{border-image: url("+QCoreApplication::applicationDirPath()+"/icon/trans_off.png)}");
            ui->transceiver_turnon_label->setText("关闭发射机");
            ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"打开发射机");
        }
    }
}

//“打开收发机”的功能槽函数(触发延时对采集器的影响？)
void MainWindow::transceiver_Slot()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString s_dis = ini->value("/radar_information/startpoint").toString(); //从ini文件中读取起始距离
    QString s_dis1 = QString("%1").arg(s_dis.toInt(),8,16,QLatin1Char('0')); //将字符串转为16进制形式，8字节，补全0（默认为1个字符一位，所以要补成8个字节）
    //将字符串拆分为uchar
    uchar u1 = uchar(s_dis1.left(2).toInt(nullptr,16));
    uchar u2 = uchar(s_dis1.left(4).right(2).toInt(nullptr,16));
    uchar u3 = uchar(s_dis1.right(4).left(2).toInt(nullptr,16));
    uchar u4 = uchar(s_dis1.right(2).toInt(nullptr,16));
    //float maximum = ini->value("/radar_information/maximum").toInt()*5/4;
    QString e_dis = ini->value("/radar_information/endpoint").toString(); //从ini文件中读取截止距离
    QString e_dis1 = QString("%1").arg(e_dis.toInt(),8,16,QLatin1Char('0')); //将字符串转为16进制形式，8字节，补全0（默认为1个字符一位，所以要补成8个字节）
    //将字符串拆分为uchar
    uchar w1 = uchar(e_dis1.left(2).toInt(nullptr,16));
    uchar w2 = uchar(e_dis1.left(4).right(2).toInt(nullptr,16));
    uchar w3 = uchar(e_dis1.right(4).left(2).toInt(nullptr,16));
    uchar w4 = uchar(e_dis1.right(2).toInt(nullptr,16));
    int sample = ini->value("/radar_information/samplestep").toDouble()/1.25;
    u_char step; //采样步长
    qDebug() << sample <<ini->value("/radar_information/samplestep").toInt();
    switch(sample)
    {
    case 1:
        step = 0x00; //1.25
        break;
    case 2:
        step = 0x01; //2.5
        break;
    case 3:
        step = 0x02; //3.75
        break;
    case 4:
        step = 0x03; //5
        break;
    case 5:
        step = 0x04; //6.25
        break;
    case 6:
        step = 0x05; //7.5
        break;
    case 8:
        step = 0x07; //10
        break;
    case 12:
        step = 0x0b; //15
        break;
    }

    pcap_t *fp;
    char errbuf[PCAP_ERRBUF_SIZE];
    QString name_all = ini->value("/NICname_choosen/nicname").toString(); //从初始文件中获取的网卡名称
    QStringList list = name_all.split("}"); //由于初始文件中保存的网卡名称包含了网卡名称和网卡描述两个部分，因此需要分开，并将网卡名称部分提取出来
    QString nicname = list.at(0)+"}"; //以}为分界点，划分时}符号归为后一部分
    QByteArray name_bytearray = nicname.toLatin1(); //先将字符串类型转换为二进制流的形式，不可一次性转为char，转换不成功
    char *nic = name_bytearray.data(); //将网卡名称转为char类型，便于下面使用

    /* 打开输出设备 */
    if ( (fp= pcap_open_live(nic,1,5,1000,errbuf))==nullptr)
    {
        QMessageBox::warning(nullptr,"网口不可用","网口打开失败，请检查相关网口设置，确保网卡选择正确。",QMessageBox::Ok,0);
        return;
    }
    u_char packet[29] = {0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x01,0x02,0x03,0x04,0x05,0x06,0xa5,0x00,0x01,0x07,0x00,0x00,0x00,0x00,0xb8,0x0b,0x00,0x00,0x02,0x41,0x31,0x37,0x03};
    /* 发送数据包 */
    if (pcap_sendpacket(fp, packet, 29 /* size */) != 0)
    {
        QMessageBox::warning(nullptr,"收发机操作失败","收发机操作失败，请检查相关硬件连接及网口设置后重新尝试。",QMessageBox::Ok,0);
        return;
    }
    QElapsedTimer t;
    t.start(); //50ms计时开始
    while(t.elapsed() < 50)
        QCoreApplication::processEvents();
    u_char packet2[29] = {0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x01,0x02,0x03,0x04,0x05,0x06,0xa5,0x00,0x81,0x07,0x00,0x00,0x00,0x00,0xb8,0x0b,0x00,0x00,0x02,0x41,0x31,0x37,0x03};
    /* 发送数据包 */
    packet2[15] = step; //0x05
    packet2[16] = u4; //00
    packet2[17] = u3; //00
    packet2[18] = u2; //00
    packet2[19] = u1; //00
    packet2[20] = w4; //196
    packet2[21] = w3; //09
    packet2[22] = w2; //00
    packet2[23] = w1; //00
    if (pcap_sendpacket(fp, packet2, 29 /* size */) != 0)
    {
        QMessageBox::warning(nullptr,"收发机操作失败","收发机操作失败，请检查相关硬件连接及网口设置后重新尝试。",QMessageBox::Ok,0);
        return;
    }
    global::transceiver_turnon_bool = !global::transceiver_turnon_bool; //此时成功对发射机进行操作，更改发射机状态变量
}

//发射机打开时状态指示灯改变的功能槽函数
void MainWindow::transmiton_status_Slot()
{
    ui->emission_status_value->setStyleSheet(m_green_SheetStyle);
}

//发射机关闭时状态指示灯改变的功能槽函数
void MainWindow::transmitoff_status_Slot()
{
    ui->emission_status_value->setStyleSheet(m_red_SheetStyle);
}


void MainWindow::chart_update()
{
    qDebug() << "发送信号成功";
    //chart1->legend()->hide();
}

//用于接收来自playback界面的信号，当龙口数据回放完毕后，将计算结果值显示在主界面上(RRAW格式也同样适用)
void MainWindow::receive_longkou_figureresult_Slot(QString r5,double r6,double r7,double r8,double r1,double r2,double r3,double r4,double *buf1,double *buf2,double *buf3,double *buf4)
{
    qDebug() << "此函数调用成功" << r4;
    //cal_peak_frequency_results->setText(QString::number(r1,'f',2)); //波峰频率

    //ui->tab31->setItem(7,0,item31V);
    //ui->tab31->setItem(4,1,item22V);
    //ui->tab31->setModel(model);
    ui->item22V->setText(QString::number(r1,'f',2)); //波峰周期
    ui->item31V->setText(QString::number(r2,'f',1)); //波峰波向
    ui->item21V->setText(QString::number(r3,'f',2)); //有效波高
    ui->item1V->setText(r5); //数据时间
    ui->item32V->setText(QString::number(r4,'f',2)); //峰波波长
    ui->item41V->setText(QString::number(r7,'f',2)); //流速
    ui->item42V->setText(QString::number(r8,'f',2)); //流向
    ui->item1V->update();
    //ui->tab31->update();
    memcpy(&fre_cor,buf1,128*sizeof(double));
    memcpy(&wave_spe,buf2,128*sizeof(double));
    memcpy(&wave_dir,buf3,128*sizeof(double));
    memcpy(&wavenum_fre,buf4,128*128*sizeof(double)); //都是1e-06
    series1->clear();
    series1->setPointsVisible(false);
    series2->clear();
    series2->setPointsVisible(false);
    series3_1->clear();
    series3_1->setPointsVisible(true);
    series3_2->clear();
    series3_2->setPointsVisible(true);
    for(int i=0;i<128;i++)
    {
        series1->append(fre_cor[i]*10,wave_spe[i]);
        //point1.push_back(QPoint(fre_cor[i],wave_spe[i]));
        //point2.push_back(QPoint(fre_cor[i],wave_dir[i]));
    }
    for(int i=0;i<128;i++)
    {
        series2->append(fre_cor[i]*10,wave_dir[i]);
    }
    /*
    for(int i=0;i<128;i++)
    {
        for(int j=0;j<128;j++)
        {
            if(i<25)
                series3_1->append(i,j);
            else if(i < 50)
                series3_2->append(i,j);
            else if(i<75)
                series3_3->append(i,j);
            else if(i<100)
                series3_4->append(i,j);
            else if(i>100)
                series3_5->append(i,j);
        }
    }*/
    /*series3_1->append(1,1);
    series3_1->append(5,1);
    series3_1->append(7,10);
    series3_2->append(1,100);
    series3_2->append(5,60);
    series3_2->append(7,9);*/

    chart1->update();
    //chart3->update();
}

//接收来自opengl界面显示时传来的实时改变主界面上的“当前方位”值的槽函数
void MainWindow::receive_change_current_orientation_Slot(float angle)
{
    current_orientation_value->setText(QString::number(angle));
}

//接收来自opengl界面交换内存时传来的信号，用以更新界面上当前帧数的槽函数
void MainWindow::receive_current_frame_Slot()
{
    current_frame++; //当前帧数加一
    ui->current_frame_value->setText(QString::number(current_frame)); //将当前帧数显示到标签中
    ui->total_frame_value->setText("64"); //总帧数设置为64
}

//接收来自paintonline类点击“停止回放”的槽函数，用于将当前帧数和总帧数置为0
void MainWindow::receive_online_frame_Slot(int count)
{
    qDebug() << "主线程接收到paintonline槽函数" << count;
    ui->current_frame_value->setText(QString::number(count)); //将当前帧数显示到标签中
    ui->total_frame_value->setText("64"); //总帧数设置为64
}

void MainWindow::receive_online_result_Slot(QString time,double f1,double f2,double f3,double f4,double *f5,double *f6,double *f7,double *f8)
{
    //f8为海浪谱，暂时不要
    //item1V->setText(time);
    //cal_peak_frequency_results->setText(QString::number(f1,'f',2)); //波峰频率
    //item22V->setText(QString::number(f3,'f',2)); //波峰周期
    //item31V->setText(QString::number(f2,'f',1)); //波峰波向
    //item21V->setText(QString::number(f4,'f',2)); //有效波高
    memcpy(&fre_cor,f5,128*sizeof(double));
    memcpy(&wave_spe,f6,128*sizeof(double));
    memcpy(&wave_dir,f7,128*sizeof(double));
    series1->clear();
    //series1->setPointLabelsVisible(true);
    series1->setPointsVisible(true);
    series2->clear();
    series2->setPointsVisible(true);
    for(int i=0;i<128;i++)
    {
        series1->append(fre_cor[i]*10,wave_spe[i]*10);
    }
    for(int i=0;i<128;i++)
    {
        series2->append(fre_cor[i]*10,wave_dir[i]);
    }
    //ui->tab31->update();
    chart1->update();
}

void MainWindow::receive_batch_result(double f1,double f2,double f3,double f4)
{
    //cal_peak_frequency_results->setText(QString::number(f1,'f',2)); //波峰频率
    //item22V->setText(QString::number(f3,'f',2)); //波峰周期
    //item31V->setText(QString::number(f2,'f',1)); //波峰波向
    //item21V->setText(QString::number(f4,'f',2)); //有效波高
    //ui->tab31->update();
}

//接收来自playback类点击“停止回放”的槽函数，用于将当前帧数和总帧数置为0
void MainWindow::receive_current_frame_clear_Slot()
{
    current_frame = 0; //将当前帧数置为0
    ui->current_frame_value->setText(QString::number(current_frame)); //将当前帧数设置为0后显示到标签中
    ui->total_frame_value->setText("...");
}



//接收来自数据回放线程龙口数据改变主界面上显示“采样步长”的槽函数
void MainWindow::receive_longkou_sampling_step_Slot()
{
    ui->minidistance_v->setText("0");
    ui->maxidistance_v->setText("6000");
    ui->sampling_step_size_value->setText("7.5");
}



//当在颜色设置界面点击保存时，将主界面上的强度值和颜色值改为对应值
void MainWindow::color_set_change_Slot()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString change_text = ini->value("/color_strength/a").toString(); //从初始文件中读取出最大可观测强度值
    QString mini_change_text = ini->value("/color_strength/b").toString(); //从初始文件中读取出最小可观测强度值
    int change_int = change_text.toInt(); //将最大值转为int类型，便于后面计算
    int mini_change_int = mini_change_text.toInt(); //将最小值转为int类型，便于后面计算

    //设置13个可观测强度值的标签
    for(int i=0;i<17;i++)
    {
        if(i==0)
        {
            numberlabel->setText(change_text); //最大可观测强度值
        }
        else if(i==16)
        {
            (numberlabel+i)->setText(mini_change_text); //最小可观测强度值
        }
        else
        {
            (numberlabel+i)->setText(QString::number(int(double((change_int-mini_change_int)/16.0)*double(16.0-i))+mini_change_int)); //显示其余的可观测强度值
        }
    }

    //设置15个色度条的颜色框
    for(int i=0;i<16;i++)
    {
        QString b('a'+i); //实现小写字母从a到m的循环
        //color_R_int = ini->value(("/color/"+b+"R").toLatin1()).toInt(); //将初始文件中的第一个颜色的R通道进行读取
        //color_G_int = ini->value(("/color/"+b+"G").toLatin1()).toInt(); //将初始文件中的第一个颜色的G通道进行读取
        //color_B_int = ini->value(("/color/"+b+"B").toLatin1()).toInt(); //将初始文件中的第一个颜色的B通道进行读取
        //background_color.setColor(QPalette::Background,QColor(color_R_int,color_G_int,color_B_int)); //设置模板的颜色
        (colorLabel+i)->setAutoFillBackground(true); //自动填满，否则无法实现
        (colorLabel+i)->setPalette(ini->value("/color/"+b).value<QPalette>()); //给Label设置模板
        (colorLabel+i)->setFixedSize(40,30); //设置显示颜色的label的大小
    }
}


//系统信息部分的初始化显示内容
void MainWindow::system_log(QGroupBox *groupbox42)
{
    QGroupBox *system_log_widget = new QGroupBox;
    system_log_widget = groupbox42; //将widget42传入函数中

    QDateTime time = QDateTime::currentDateTime(); //获取现在的时间
    ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"软件开始运行"); //输出软件开始运行的信息
    QSettings *browser_ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat); //从ini文件中获取当前的采集方式
    int browser_Id = browser_ini->value("/collection_mode/Id").toInt();
    switch (browser_Id) //根据Id来判断当前的运行方式
    {
    case 0:
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"手动采集模式开启");
        break;
    case 1:
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"每小时平均采集模式开启");
        break;
    case 2:
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"累计平均模式开启");
        break;
    default:
        break;
    }
    int auto_store_checkbox = browser_ini->value("/auto_store_rawdata_checkbox/check").toInt();
    if(auto_store_checkbox) //显示是否开启了自动存储原始数据模式
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"自动存储原始数据模式开启");
    }
    else
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"自动存储原始数据模式关闭");
    }
    //system_log_widget->setLayout(vlayout); //设置此区域的布局
    //vlayout->addWidget(ui->browser);
}

//当"运行设置"保存时调用的用以更新系统信息的槽函数
void MainWindow::receive_runset_to_loginformation_Slot()
{
    QDateTime time = QDateTime::currentDateTime(); //获取现在的时间
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat); //从ini文件中获取当前的采集方式
    int Id = ini->value("/collection_mode/Id").toInt();
    switch (Id) //根据Id来判断当前的运行方式
    {
    case 0:
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"手动采集模式开启");
        ui->acquisition_mode_v->setText("手动采集");
        break;
    }
    case 1:
    {
        ui->acquisition_mode_v->setText("每小时平均采集");
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"每小时平均采集模式开启");
        break;
    }
    case 2:
    {
        ui->acquisition_mode_v->setText("累计平均采集");
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"累计平均模式开启");
        break;
    }
    default:
        break;
    }
    int auto_store_checkbox = ini->value("/auto_store_rawdata_checkbox/check").toInt();
    if(auto_store_checkbox) //显示是否开启了自动存储原始数据模式
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"自动存储原始数据模式开启");
    }
    else
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"自动存储原始数据模式关闭");
    }

    ui->save_frame_v->setText(ini->value("/collection_mode/framenumber").toString()); //存储帧数
}

//当点击开始处理和停止处理时用以更新系统信息的槽函数
void MainWindow::receive_startplay_Slot()
{
    QDateTime time = QDateTime::currentDateTime(); //获取现在的时间
    if(global::start_or_stop_dealing)
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"开始处理");
    }
    else if(global::start_or_stop_dealing == false)
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"停止处理");
    }
}

//接收从开始回放界面传来的改变系统信息的槽函数
void MainWindow::receive_start_playback_Slot()
{
    QDateTime time = QDateTime::currentDateTime(); //获取现在的时间
    if(playback_static_varying == 1)
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"开始回放");
    }
    else if(playback_static_varying == 2)
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"停止回放");
    }
    else if(playback_static_varying == 3)
    {
        ui->browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"保存并退出回放界面");
    }
}

//设置计算区域部分radiobutton的槽函数
void MainWindow::area_Slot()
{
    switch (radioButtonGroup->checkedId()) {
    case 0:
        global::area_hiding = false;
        global::area_display = true;
        ui->widget->area_choose(); //调用opengl界面的刷新函数
        break;
    case 1:
        global::area_hiding = true;
        global::area_display = false;
        ui->widget->area_choose(); //调用opengl界面的刷新函数
        break;
    default:
        break;
    }
}

//当系统设置界面关闭时，调用此函数改变主界面上显示的采样步长，起始和结束观测距离
void MainWindow::update_radarinform_Slot()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    ui->sampling_step_size_value->setText(ini->value("/radar_information/samplestep").toString());
    ui->minidistance_v->setText(ini->value("/radar_information/minimum").toString()); //起始距离
    ui->maxidistance_v->setText(ini->value("/radar_information/maximum").toString()); //结束距离
    ui->sampling_step_size_value->setText(ini->value("/radar_information/samplestep").toString()); //采样步长
    ui->trigger_delay_v->setText(ini->value("/radar_information/triggerdelay").toString()); //触发延时
    ui->base_noise_v->setText(ini->value("/radar_information/noisebasevalue").toString()); //噪声基底参数
    int site1 = ini->value("/site_information/onBoard").toInt(); //站点类型
    int site2 = ini->value("/site_information/shoreBased").toInt();
    if(site1 == 1)
        ui->site_type_v->setText("船载");
    else if(site2 == 1)
        ui->site_type_v->setText("岸基");
    ui->tdomain_points_v->setText(ini->value("/calculation_parameter/time_domain_points").toString()); //时域点数
    ui->airx_points_v->setText(ini->value("/calculation_parameter/airspace_points_x").toString()); //Nx
    ui->airy_points_v->setText(ini->value("/calculation_parameter/airspace_points_y").toString()); //Ny
    //correct_angle_v->setText(ini->value("/calculation_parameter/radar_correction_angle").toString() + "度"); //修正角度
    ui->water_deep_v->setText(ini->value("/calculation_parameter/water_depth").toString() + "米"); //水深
    ui->dertat_v->setText(ini->value("/calculation_parameter/sampling_interval_time").toString()); //时间采样间隔
    ui->dertax_v->setText(ini->value("/calculation_parameter/sampling_interval_x").toString()); //x抽样间隔
    ui->dertay_v->setText(ini->value("/calculation_parameter/sampling_interval_y").toString()); //y抽样间隔
}

//当系统打开时、系统设置界面关闭时，调用此函数向采集盒发送控制采样步长、起始和结束距离的指令

//当计算区域选择界面保存时，调用此函数，改变主界面上显示的计算区域
//由于在保存时，在计算区域部分的函数中已经判断过各值，所以可以放心使用
void MainWindow::calculate_area_change_Slot()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    if(ini->value("/area/id").toInt() == 0) //当id值为0时，选用的是自定义模式输入
    {
        ui->angle_min->setText(ini->value("/area/angle1").toString()); //读出最小角度
        ui->angle_max->setText(ini->value("/area/angle2").toString()); //读出最大角度
        ui->radio_min->setText(ini->value("/area/radio1").toString()); //读出最小半径
        ui->radio_max->setText(ini->value("/area/radio2").toString()); //读出最大半径
    }
    else if(ini->value("/area/id").toInt() == 1) //当id值为1时，选用的是固定大小模式输入
    {
        int point_angle = ini->value("/area/center_angle").toInt(); //中心点的角度
        int point_radio = ini->value("/area/center_radio").toInt(); //中心点的半径
        int angle_range = ini->value("/area/angle_range").toInt(); //计算区域的角度范围
        int radio_range = ini->value("/area/radio_range").toInt(); //计算区域的半径范围
        ui->radio_min->setText(QString::number(point_radio-(radio_range/2))); //计算出最小半径
        ui->radio_max->setText(QString::number(point_radio+(radio_range/2))); //计算出最大半径
        if(point_angle - (angle_range/2)<0) //判断最小角度是否小于0（跨零）
        {
            ui->angle_min->setText(QString::number(360 - abs(point_angle - (angle_range/2))));
        }
        else
        {
            ui->angle_min->setText(QString::number(point_angle - (angle_range/2)));
        }
        if(point_angle+angle_range/2>360) //判断最大角度是否大于360度（跨零）
        {
            ui->angle_max->setText(QString::number(point_angle+angle_range/2-360));
        }
        else
        {
            ui->angle_max->setText(QString::number(point_angle+angle_range/2));
        }
    }
}

//当回放部分在回放平潭数据时，发送来信号根据数据文件头信息更改显示观测范围
void MainWindow::pingtan_distance_change()
{
    qDebug() << "mainwindow:接收到来自子线程的信号";
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString input_mini = ini->value("/radar_information/minimum").toString(); //读取显示观测范围的最小值
    QString input_maxi = ini->value("/radar_information/maximum").toString(); //读取显示观测范围的最大值
    ui->minidistance_v->setText(input_mini); //改变显示观测范围的最小值
    ui->maxidistance_v->setText(input_maxi); //改变显示观测范围的最大值
    //area_returnPressed_Slot(); //调用计算区域选择部分的函数，重新判断计算区域的值是否符合要求
    ui->sampling_step_size_value->setText("7.5");
}

//系统菜单-开始处理
void MainWindow::startprocessingSlot()
{
    //当用户点击时的状态为“开始处理”时
    if(global::start_or_stop_dealing)
    {
        /*if(!(power_radar_bool && transceiver_turnon_bool)) //当未打开雷达或未打开发射机时
        {
            QMessageBox::warning(this,"未打开雷达或发射机","请先打开雷达和发射机",QMessageBox::Ok);
            return;
        }*/
        receive_startplay_Slot(); //更新系统信息的数据
        global::start_or_stop_dealing = false; //改变此时按钮的状态
        global::start_running_thread = true; //使得抓包线程中的耗时函数循环处于开启的状态
        global::ins_start = true; //使得惯导数据读取线程中的循环开始
        ui->action11->setIcon(QIcon(QCoreApplication::applicationDirPath()+"/icon/stop.png"));
        ui->action11->setText("停止处理");
        qDebug() << "开始处理的槽函数调用";
        emit insinitial_thread(); //发送将惯导初始化的信号
    }
    else
    {
        receive_startplay_Slot(); //更新系统信息的数据
        global::start_or_stop_dealing = true; //改变此时按钮的状态
        global::start_running_thread = false; //使得多线程中的耗时函数停止运行
        global::ins_start = false; //使得惯导数据读取线程中的循环停止
        ui->action11->setIcon(QIcon(QCoreApplication::applicationDirPath()+"/icon/play.png"));
        ui->action11->setText("开始处理");
    }
}

//惯导的函数开启其他线程的槽函数
void MainWindow::start_otherthread_Slot()
{
    qDebug() << "开启其他线程的槽函数";
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int ins = ini->value("/ins_com/check").toInt(); //判断是否有惯导的输入
    //当有惯导输入时，调用惯导线程中的循环函数
    if(ins)
        emit start_ins_thread();
    emit start_packet_data_thread(); //发送抓包数据的信号
    //emit start_harddisk_thread(); //发送将数据存入硬盘的信号
    emit paint_online_signal_thread(); //发送实时画图的信号
    emit figure_initial_thread(); //发送区域计算的信号
}

//数据抓包线程中的信号对应的槽函数
void MainWindow::start_process_receiveMessageSlot()
{
    qDebug() << "主线程中获得“开始处理”线程中的消息";
}

//存入硬盘线程中信号对应的槽函数
void MainWindow::start_saving_receiveMessageSlot()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString frame_num = ini->value("/collection_mode/framenumber").toString();
    ui->total_frame_value->setText(frame_num);
    qDebug() << "主线程中获得“将数据存入硬盘”线程中的消息";
}

//区域插值计算线程中信号对应的槽函数
void MainWindow::area_calculation_receiveMessageSlot()
{
    qDebug() << "主线程中获得“区域插值计算”线程中的消息";
}

//实时画图线程中信号对应的槽函数
void MainWindow::paint_online_receiveMessageSlot()
{
    qDebug() << "主线程中获得“实时绘制图像”线程中的消息";
}

//接收来自paintonline实时部分的惯导数据，并显示在主界面上
void MainWindow::onlinenavi_show(int longitude,int latitude,double shipspeed,double shipdirection)
{
    if(longitude == latitude == shipspeed == shipdirection == 0)
        ui->insStatus_v->setStyleSheet(m_red_SheetStyle);
    else
    ui->insStatus_v->setStyleSheet(m_green_SheetStyle);
    ui->Longitude_v->setText(QString::number(longitude,'f',3)); //经度
    ui->latitude_v->setText(QString::number(latitude,'f',3)); //纬度
    ui->shipSpeed_v->setText(QString::number(shipspeed,'f',3)); //船速
    ui->shipDirection_v->setText(QString::number(shipdirection,'f',3)); //船向
    qDebug() << "!!!!!!!" << longitude << latitude << shipspeed << shipdirection;
}

void MainWindow::receive_rrawInsShow(double d1,double d2,double d3,double d4)
{
    /*if(d1 == d2 == d3 == d4 == 0)
        insStatus_v->setStyleSheet(m_red_SheetStyle);
    else
        insStatus_v->setStyleSheet(m_green_SheetStyle);*/
    ui->Longitude_v->setText(QString::number(d1,'f',1)); //经度
    ui->latitude_v->setText(QString::number(d2,'f',1)); //纬度
    ui->shipSpeed_v->setText(QString::number(d3,'f',2)); //船速
    ui->shipDirection_v->setText(QString::number(d4,'f',2)); //船向
    //qDebug() << "!!!!!!!" << d1 << d2 << d3 << d4;
}

void MainWindow::databaseError_Slot()
{
    QDateTime time = QDateTime::currentDateTime(); //获取现在的时间
    browser->append(time.toString("yyyy-MM-dd hh:mm:ss")+" "+"数据库存入错误"); //输出数据库报错信息
}

//设置主界面色度条和强度值的函数
//ini文件的初始化设置
void MainWindow::glGR(QGridLayout *gl2GR)
{
    QGridLayout *color_gridLayout = new QGridLayout; //将色度条部分的布局传入函数
    color_gridLayout = gl2GR;
    color_gridLayout->setSpacing(0); //设置网格间隔为0

    if(firsttime_run_forini)
    {
        //判断设置路径下是否存在两个ini文件，若不存在时新建并输入初始值，存在时跳过
        QDir tempDir;
        QString currentDir = QCoreApplication::applicationDirPath();
        if(!tempDir.exists(currentDir))
        {
            tempDir.mkpath(currentDir); //生成此路径
        }

        tempDir.setCurrent(currentDir);

        //当该路径下此文件不存在时，新建文件名为Filename2的初始文件
        QFile *iniFile2 = new QFile;
        if(!iniFile2->exists(Filename))
        {
            iniFile2->setFileName(Filename);
            if(!iniFile2->open(QIODevice::WriteOnly))
            {
                qDebug() << QObject::tr("打开失败2");
            }
            QTextStream out(iniFile2);
            out << "[color_strength]" << endl;
            out << "a=6000" << endl;
            out << "b=0" << endl;
            out << "[color2]" << endl;
            out << "aR=0" << endl;
            out << "aG=255" << endl;
            out << "aB=0" << endl;
            out << "bR=0" << endl;
            out << "bG=238" << endl;
            out << "bB=0" << endl;
            out << "cR=0" << endl;
            out << "cG=221" << endl;
            out << "cB=0" << endl;
            out << "dR=0" << endl;
            out << "dG=204" << endl;
            out << "dB=0" << endl;
            out << "eR=0" << endl;
            out << "eG=187" << endl;
            out << "eB=0" << endl;
            out << "fR=0" << endl;
            out << "fG=170" << endl;
            out << "fB=0" << endl;
            out << "gR=0" << endl;
            out << "gG=153" << endl;
            out << "gB=0" << endl;
            out << "hR=0" << endl;
            out << "hG=136" << endl;
            out << "hB=0" << endl;
            out << "iR=0" << endl;
            out << "iG=119" << endl;
            out << "iB=0" << endl;
            out << "jR=0" << endl;
            out << "jG=102" << endl;
            out << "jB=0" << endl;
            out << "kR=0" << endl;
            out << "kG=85" << endl;
            out << "kB=0" << endl;
            out << "lR=0" << endl;
            out << "lG=68" << endl;
            out << "lB=0" << endl;
            out << "mR=0" << endl;
            out << "mG=51" << endl;
            out << "mB=0" << endl;
            out << "nR=0" << endl;
            out << "nG=34" << endl;
            out << "nB=0" << endl;
            out << "oR=0" << endl;
            out << "oG=17" << endl;
            out << "oB=0" << endl;
            out << "pR=0" << endl;
            out << "pG=0" << endl;
            out << "pB=0" << endl;

            out << "[color3]" << endl;
            out << "aR=255" << endl;
            out << "aG=255" << endl;
            out << "aB=0" << endl;
            out << "bR=238" << endl;
            out << "bG=238" << endl;
            out << "bB=0" << endl;
            out << "cR=221" << endl;
            out << "cG=221" << endl;
            out << "cB=0" << endl;
            out << "dR=204" << endl;
            out << "dG=204" << endl;
            out << "dB=0" << endl;
            out << "eR=187" << endl;
            out << "eG=187" << endl;
            out << "eB=0" << endl;
            out << "fR=170" << endl;
            out << "fG=170" << endl;
            out << "fB=0" << endl;
            out << "gR=153" << endl;
            out << "gG=153" << endl;
            out << "gB=0" << endl;
            out << "hR=136" << endl;
            out << "hG=136" << endl;
            out << "hB=0" << endl;
            out << "iR=119" << endl;
            out << "iG=119" << endl;
            out << "iB=0" << endl;
            out << "jR=102" << endl;
            out << "jG=102" << endl;
            out << "jB=0" << endl;
            out << "kR=85" << endl;
            out << "kG=85" << endl;
            out << "kB=0" << endl;
            out << "lR=68" << endl;
            out << "lG=68" << endl;
            out << "lB=0" << endl;
            out << "mR=51" << endl;
            out << "mG=51" << endl;
            out << "mB=0" << endl;
            out << "nR=34" << endl;
            out << "nG=34" << endl;
            out << "nB=0" << endl;
            out << "oR=17" << endl;
            out << "oG=17" << endl;
            out << "oB=0" << endl;
            out << "pR=0" << endl;
            out << "pG=0" << endl;
            out << "pB=0" << endl;

            out << "[color4]" << endl;
            out << "aR=0" << endl;
            out << "aG=0" << endl;
            out << "aB=255" << endl;
            out << "bR=0" << endl;
            out << "bG=0" << endl;
            out << "bB=238" << endl;
            out << "cR=0" << endl;
            out << "cG=0" << endl;
            out << "cB=221" << endl;
            out << "dR=0" << endl;
            out << "dG=0" << endl;
            out << "dB=204" << endl;
            out << "eR=0" << endl;
            out << "eG=0" << endl;
            out << "eB=187" << endl;
            out << "fR=0" << endl;
            out << "fG=0" << endl;
            out << "fB=170" << endl;
            out << "gR=0" << endl;
            out << "gG=0" << endl;
            out << "gB=153" << endl;
            out << "hR=0" << endl;
            out << "hG=0" << endl;
            out << "hB=136" << endl;
            out << "iR=0" << endl;
            out << "iG=0" << endl;
            out << "iB=119" << endl;
            out << "jR=0" << endl;
            out << "jG=0" << endl;
            out << "jB=102" << endl;
            out << "kR=0" << endl;
            out << "kG=0" << endl;
            out << "kB=85" << endl;
            out << "lR=0" << endl;
            out << "lG=0" << endl;
            out << "lB=68" << endl;
            out << "mR=0" << endl;
            out << "mG=0" << endl;
            out << "mB=51" << endl;
            out << "nR=0" << endl;
            out << "nG=0" << endl;
            out << "nB=34" << endl;
            out << "oR=0" << endl;
            out << "oG=0" << endl;
            out << "oB=17" << endl;
            out << "pR=0" << endl;
            out << "pG=0" << endl;
            out << "pB=0" << endl;

            out << "[color]" << endl;
            out << "aR=128" << endl;
            out << "aG=0" << endl;
            out << "aB=0" << endl;
            out << "bR=255" << endl;
            out << "bG=0" << endl;
            out << "bB=0" << endl;
            out << "cR=255" << endl;
            out << "cG=128" << endl;
            out << "cB=0" << endl;
            out << "dR=255" << endl;
            out << "dG=255" << endl;
            out << "dB=0" << endl;
            out << "eR=128" << endl;
            out << "eG=255" << endl;
            out << "eB=0" << endl;
            out << "fR=0" << endl;
            out << "fG=255" << endl;
            out << "fB=0" << endl;
            out << "gR=0" << endl;
            out << "gG=255" << endl;
            out << "gB=128" << endl;
            out << "hR=0" << endl;
            out << "hG=255" << endl;
            out << "hB=255" << endl;
            out << "iR=0" << endl;
            out << "iG=128" << endl;
            out << "iB=255" << endl;
            out << "jR=0" << endl;
            out << "jG=0" << endl;
            out << "jB=255" << endl;
            out << "kR=0" << endl;
            out << "kG=0" << endl;
            out << "kB=128" << endl;
            out << "lR=0" << endl;
            out << "lG=0" << endl;
            out << "lB=0" << endl;
            out << "mR=0" << endl;
            out << "mG=0" << endl;
            out << "mB=0" << endl;
            out << "nR=0" << endl;
            out << "nG=0" << endl;
            out << "nB=0" << endl;
            out << "oR=0" << endl;
            out << "oG=0" << endl;
            out << "oB=0" << endl;
            out << "pR=0" << endl;
            out << "pG=0" << endl;
            out << "pB=0" << endl;

            out << "a=" << endl;
            out << "b=" << endl;
            out << "c=" << endl;
            out << "d=" << endl;
            out << "e=" << endl;
            out << "f=" << endl;
            out << "g=" << endl;
            out << "h=" << endl;
            out << "i=" << endl;
            out << "j=" << endl;
            out << "k=" << endl;
            out << "l=" << endl;
            out << "m=" << endl;
            out << "n=" << endl;
            out << "o=" << endl;
            out << "p=" << endl;
            out << "[review_file]" << endl;
            out << "filename=C:/Users/" << endl;
            out << "[dataformat]" << endl;
            out << "format=" << endl;
            out << "[area]" << endl;
            out << "id=0" << endl;
            out << "radio1=1200" << endl;
            out << "radio2=2000" << endl;
            out << "angle1=20" << endl;
            out << "angle2=100" << endl;
            out << "center_angle=0" << endl;
            out << "center_radio=1000" << endl;
            out << "angle_range=0" << endl;
            out << "radio_range=0" << endl;
            out << "[Waveform_display]" << endl;
            out << "id=0" << endl;
            out << "angle=0" << endl;
            out << "[radar_information]" << endl;
            out << "samplestep=7.5" << endl;
            out << "minimum=0" << endl;
            out << "maximum=6000" << endl;
            out << "noisebasevalue=" << endl;
            out << "triggerdelay=" << endl;
            out << "openglAngle=" << endl;
            out << "gatestep=" << endl;
            out << "grade=" << endl;
            out << "state=" << endl;
            out << "startpoint=" << endl;
            out << "endpoint=" << endl;
            out << "[NICname_choosen]" << endl;
            out << "nicname=nochoose" << endl;
            out << "[auto_store_rawdata_checkbox]" << endl;
            out << "check=1" << endl;
            out << "[collection_mode]" << endl;
            out << "Id=0" << endl;
            out << "collection_times=1" << endl;
            out << "cycle=30" << endl;
            out << "storage_interval=0" << endl;
            out << "framenumber=64" << endl;
            out << "[initialPath]" << endl;
            out << "savepath=E:" << endl;
            out << "[storage_path_of_calculation_results]" << endl;
            out << "calsavepath=E:" << endl;
            out << "[site_information]" << endl;
            out << "name=" << endl;
            out << "englishabbr=" << endl;
            out << "sitenumber=" << endl;
            out << "onBoard=" << endl;
            out << "shoreBased=" << endl;
            out << "[calculation_parameter]" << endl;
            out << "radar_correction_angle=" << endl;
            out << "water_depth=" << endl;
            out << "ship_high=" << endl;
            out << "time_domain_points=" << endl;
            out << "airspace_points_x=" << endl;
            out << "airspace_points_y=" << endl;
            out << "sampling_interval_time=" << endl;
            out << "sampling_interval_x=" << endl;
            out << "sampling_interval_y=" << endl;
            out << "[batchprocess_path]" << endl;
            out << "path=" << endl;
            out << "[batchprocess]" << endl;
            out << "txt=" << endl;
            out << "txtPath=" << endl;
            out << "xlsx=" << endl;
            out << "xlsxPath=" << endl;
            out << "xlsxName=" << endl;
            out << "[ins_com]" << endl;
            out << "port=" << endl;
            out << "baud=115200" << endl;
            out << "bit=8" << endl;
            out << "parity=1" << endl;
            out << "stop=" << endl;
            out << "check=" << endl;
            out << "[out_com]" << endl;
            out << "port=" << endl;
            out << "baud=115200" << endl;
            out << "bit=8" << endl;
            out << "parity=1" << endl;
            out << "stop=" << endl;
            out << "check=" << endl;
            iniFile2->close();
        }
        firsttime_run_forini = false;
    }
/*
    //打开ini文件
    QSettings *color_number_ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString numberLabel1_text = color_number_ini->value("/color_strength/a").toString(); //读取初始的最大可观测强度值
    QString numberLabel2_text = color_number_ini->value("/color_strength/b").toString(); //读取初始的最小可观测强度值
    int numberLabel1_text_int = numberLabel1_text.toInt(); //将最大值转为int类型，便于后面计算
    int numberLabel2_text_int = numberLabel2_text.toInt(); //将最小值转为int类型，便于后面计算

    //设置16个可观测强度值的标签
    for(int i=0;i<17;i++)
    {
        if(i==0)
        {
            numberlabel->setText(numberLabel1_text); //设置显示最大可观测强度值
        }
        else if(i==16)
        {
            (numberlabel+i)->setText(numberLabel2_text); //设置显示最大可观测强度值
        }
        else
        {
            (numberlabel+i)->setText(QString::number(int(double((numberLabel1_text_int-numberLabel2_text_int)/16.0)*double(16.0-i))+numberLabel2_text_int)); //显示其余的可观测强度值
        }
        color_gridLayout->addWidget((numberlabel+i),i*4,1,4,1); //设置显示位置（显示对象，开始行数，开始列数，所占行数，所占列数）
    }

    //设置15个色度条的颜色框
    for(int i=0;i<16;i++)
    {
        QString b('a'+i); //实现小写字母从a到m的循环
        (colorLabel+i)->setAutoFillBackground(true); //自动填满，否则无法实现
        (colorLabel+i)->setPalette(color_number_ini->value("/color/"+b).value<QPalette>()); //给Label设置模板
        (colorLabel+i)->setFixedSize(40,30); //设置显示颜色的label的大小
        color_gridLayout->addWidget((colorLabel+i),i*4+1,0,4,1); //将label添加到网格布局中
    }
*/
}

//设置菜单-运行设置
void MainWindow::runningSetSlot()
{
    run_set->show();
}

//系统菜单-数据回放
void MainWindow::dataReviewSlot()
{
    play_back->show();
}

//系统菜单-惯导测试
void MainWindow::instestSlot()
{
    ins_test->show();
}

//数据处理菜单-批处理
void MainWindow::batchProcSlot()
{
    batch->show();
}

//数据处理菜单-波形显示
void MainWindow::waveformSlot()
{
    //global::waveshow = true;
    wavedisplay->show();
}

//数据处理菜单-区域选择
void MainWindow::regionSelectionSlot()
{
    regionselection->show();
}

//数据处理菜单-计算结果
void MainWindow::figureResultSlot()
{
    figure_result->show();
}

//设置菜单-串口设置
void MainWindow::comsetSlot()
{
    com_Set->show();
}

//设置菜单-颜色设置
void MainWindow::ColorSetSlot()
{
    color_Set->show();
}

//设置菜单-系统设置
void MainWindow::systemsetSlot()
{
    system_Set->show();
}

//帮助菜单-帮助
void MainWindow::helpSlot()
{
    //打开软件操作说明文档
    if(!QDesktopServices::openUrl(QUrl(QCoreApplication::applicationDirPath()+"/document.pdf")))
        QMessageBox::warning(nullptr,"错误","未检测到说明文档！",QMessageBox::Ok,0);
}

//帮助菜单-关于
void MainWindow::relatedSlot()
{
    related_widget->show();

}

