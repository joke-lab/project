#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QTableWidgetItem>
#include <QObject>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QOpenGLWidget>
#include <QLabel>
#include <QFont>
#include <QSettings>
#include <QString>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QMessageBox>
#include <QTextBrowser>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QTabWidget>
#include <QChart>
#include <QLCDNumber>
#include <QtCharts>
#include <QValueAxis>
#include <QReadWriteLock>
#include <QProgressBar>
#include <QOpenGLWidget>
#include <QRegion>
#include <QDesktopServices>
#include  <QToolBar>

#include <stdlib.h>
#include <stdio.h>
#include <color_set.h>
#include <playback.h>
#include <openglwidget.h>
#include <global.h>
#include <running_set.h>
#include <startprocessing.h>
#include <startsavingtoharddisk.h>
#include <system_set.h>
#include <paintonline.h>
#include <figure.h>
#include <region_selection.h>
#include <playback_readdata.h>
#include <batchproc.h>
#include <com_set.h>
#include <instest.h>
#include <insdata.h>
#include <figureresult.h>
#include <filesdeal.h>
#include <waveview.h>
#include <related.h>
#include <commonfunction.h>
#include <QDebug>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void mouseMoveEvent(QMouseEvent *event);//鼠标移动时间显示当前鼠标坐标位置

signals:
    void insinitial_thread(); //触发惯导线程的初始化函数
    void start_figure_thread(); //触发计算线程的信号
    void start_packet_data_thread(); //触发抓包数据线程的信号
    void start_ins_thread(); //触发惯导线程循环函数的信号
    void start_harddisk_thread(); //触发存入硬盘线程的信号
    void area_interpolation_calculation_thread(); //触发区域插值计算线程的信号
    void paint_online_signal_thread(); //触发实时画图线程的信号
    void figure_initial_thread(); //触发区域计算线程的信号
    void update_openglwidget(); //发送信号调用opengl界面上的刷新函数，使其更新

private slots:
    void timeUpdate();//获取实时时间显示在主界面上
    void helpSlot(); //帮助的槽函数
    void relatedSlot(); //关于的槽函数
    void runningSetSlot(); //运行设置的槽函数
    void ColorSetSlot(); //颜色设置的槽函数
    void comsetSlot(); //串口设置的槽函数
    void startprocessingSlot(); //开始回放的槽函数
    void instestSlot(); //惯导测试的槽函数
    void dataReviewSlot(); //数据回放的槽函数
    void regionSelectionSlot(); //区域选择的槽函数
    void batchProcSlot(); //批处理的槽函数
    void figureResultSlot(); //计算结果的槽函数
    void waveformSlot(); //波形显示的槽函数
    void area_Slot();  //区域显示按钮的槽函数
    void systemsetSlot();  //系统设置的槽函数

    void open_radar_power_Slot(); //“打开雷达”图片按钮的槽函数
    void open_radar_transceiver_Slot(); //“打开收发机”图片按钮的槽函数
    void poweron_Slot(); //“打开雷达”命令发送的功能槽函数
    void poweroff_Slot(); //“关闭雷达”命令发送的功能槽函数
    void transceiver_Slot(); //“打开收发机”命令发送的功能槽函数
    void radaron_status_Slot(); //雷达打开时状态指示灯改变的功能槽函数
    void radaroff_status_Slot(); //雷达关闭s时状态指示灯改变的功能槽函数
    void transmiton_status_Slot(); //发射机打开时状态指示灯改变的功能槽函数
    void transmitoff_status_Slot(); //发射机关闭时状态指示灯改变的功能槽函数
    void update_radarinform_Slot(); //系统设置界面关闭时改变主界面显示的观测范围和采样步长的槽函数

    void start_process_receiveMessageSlot(); //开始处理线程中获得消息信号对应的槽函数
    void start_saving_receiveMessageSlot(); //将数据存入硬盘线程中获得消息信号对应的函数
    void area_calculation_receiveMessageSlot(); //区域插值计算线程中获得消息信号对应的函数
    void paint_online_receiveMessageSlot(); //实时绘图线程中获得消息信号对应的函数
    void start_otherthread_Slot(); //抓包的函数开启其他线程的槽函数
    void color_set_change_Slot(); //颜色设置界面的参数改变主界面的槽函数
    void pingtan_distance_change(); //当回放部分在回放平潭数据时，发送来信号根据数据文件头信息更改显示观测范围
    void receive_runset_to_loginformation_Slot(); //当运行设置保存时用以更新系统信息的槽函数
    void receive_startplay_Slot(); //当点击ka开始运行时用以更新系统信息的槽函数
    void receive_start_playback_Slot(); //接收从开始回放界面传来的改变系统信息的槽函数
    void receive_change_current_orientation_Slot(float angle); //接收来自opengl界面显示时传来的实时改变主界面上的“当前方位”值的槽函数
    void receive_longkou_sampling_step_Slot(); //接收来自数据回放线程改变主界面上显示“采样步长”的槽函数
    void receive_current_frame_Slot(); //接收来自opengl界面的信号，用以更新主界面上当前帧数的槽函数
    void receive_current_frame_clear_Slot(); //接收来自playback“停止回放”发来的信号，用以将当前帧数和总帧数清零
    //void receive_start_showing_Slot(); //接收来自开始处理中显示子线程发回的信号，用以开启显示主线程中的变量
    void calculate_area_change_Slot(); //接收来自区域选择界面的信号，用以改变主界面上的可观测范围的值
    void receive_longkou_figureresult_Slot(QString,double,double,double,double,double,double,double,double *,double *,double *,double *);
    void receive_rrawInsShow(double,double,double,double); //接收来自回放的rraw的信号，用以改变主界面上的INS信息
    void receive_online_result_Slot(QString,double,double,double,double,double *,double *,double *,double *);
    void receive_online_frame_Slot(int);
    void receive_batch_result(double,double,double,double);
    void chart_update();
    void onlinenavi_show(int,int,double,double); //接收来自paintonline实时部分的惯导数据
    void databaseError_Slot(); //接受来自figure线程的函数，用以在系统信息中显示数据库错误

private:
    Ui::MainWindow *ui;
    int N = 10;
    void computationalAreaSel(QGroupBox *); //计算区域选择部分的布局
    void glGLT(QGridLayout *);
    void glGRT(QGridLayout *);
    void glGLB(QGridLayout *);
    void glGRB(QGridLayout *);
    void glGR(QGridLayout *); //颜色条
    void initStatusBar(); //状态栏初始化函数

    //状态栏设置鼠标位置、旋转角度以及系统时间
    QLabel *m_statusLabel = new QLabel;
    QLabel *currentTimeLabel = new QLabel;
    //状态栏倒计时指示灯
    QLCDNumber *lcd; //显示雷达三分钟预热倒计时的显示屏
    QLabel *preheat = new QLabel; //"雷达预热倒计时："字样
    //颜色条设置
    QLabel *numberlabel = new QLabel[17]; //建立13个label用于显示可观测强度值
    QLabel *colorLabel = new QLabel[16]; //定义显示颜色的13个label
    //系统文件
    void system_log(QGroupBox *); //主界面上系统日志的部分
    QTextBrowser *browser = new QTextBrowser; //新建一个文本浏览器用于系统信息
    QVBoxLayout *vlayout = new QVBoxLayout; //系统信息模块中的布局

    QString Filename = "setting.ini"; //区域选择部分的初始文件名称
    bool firsttime_run_forini = true; //用于判断是否是第一次运行，用于ini文件是否需要初始化
    bool firsttime_run_forstartdealing = true; //用于判断是否是第一次运行，用于实时处理第一次发送信号的判断
    int current_frame = 0; //用来计数当前显示的帧数的计数器
    //雷达倒计时
    bool preheating = false; //当在三分钟倒计时时，将此变量设置为真
    //指示灯颜色
    QString m_red_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid black;background:red";
    QString m_green_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid black;background:green";

    //计算区域选择部分的radio组
    QButtonGroup *radioButtonGroup = new QButtonGroup;

    //计算结果部分的显示
    /*
    QTableWidgetItem *item1V = new QTableWidgetItem; //数据时间
    QTableWidgetItem *item21V = new QTableWidgetItem; //有效波高
    QTableWidgetItem *item22V = new QTableWidgetItem; //峰波周期
    QTableWidgetItem *item31V = new QTableWidgetItem; //峰波波向
    QTableWidgetItem *item32V = new QTableWidgetItem; //峰波波长
    QTableWidgetItem *item41V = new QTableWidgetItem; //流速
    QTableWidgetItem *item42V = new QTableWidgetItem; //流向
    */
    QLabel *current_orientation_value = new QLabel; //显示“当前方位”的标签

    //谱的显示
    QTabWidget *tabwidget = new QTabWidget;
    QtCharts::QChartView *chartview1 = new QtCharts::QChartView(this);
    QtCharts::QChart *chart1 = new QtCharts::QChart();
    QtCharts::QChartView *chartview3 = new QtCharts::QChartView;
    QtCharts::QChart *chart3 = new QtCharts::QChart;
    QValueAxis *axis_x = new QValueAxis();
    QValueAxis *axis_y = new QValueAxis();
    QValueAxis *axis_y2 = new QValueAxis();
    QLineSeries *series1 = new QLineSeries();
    QLineSeries *series2 = new QLineSeries();
    QValueAxis *axis3_x = new QValueAxis();
    QValueAxis *axis3_y = new QValueAxis();
    QLineSeries *series3_1 = new QLineSeries();
    QLineSeries *series3_2 = new QLineSeries();
    QLineSeries *series3_3 = new QLineSeries();
    QLineSeries *series3_4 = new QLineSeries();
    QLineSeries *series3_5 = new QLineSeries();
    double fre_cor[128] = {0};
    double wave_spe[128] = {0};
    double wave_dir[128] = {0};
    double wavenum_fre[128*128] = {0};
    QList<QPointF> point1;
    QList<QPointF> point2;


    region_selection *regionselection = new region_selection; //用于菜单到区域选择界面的调用
    batchproc *batch = new batchproc; //用于菜单到批处理界面的调用
    figureresult *figure_result = new figureresult; //用于菜单到计算结果界面的调用
    color_set *color_Set = new color_set; //用于菜单到颜色设置界面的调用
    playback *play_back = new playback; //用于菜单到数据回放界面的调用
    instest *ins_test = new instest; //用于菜单到惯导测试界面的调用
    filesdeal *files_deal = new filesdeal; //用于批处理子线程函数的调用
    waveview *wavedisplay; //用于波形显示界面的调用
    related *related_widget = new related; //用于关于函数的调用
    running_set *run_set = new running_set; //用于设置中运行设置的调用
    system_set *system_Set = new system_set; //用于菜单到系统设置界面的调用
    com_set *com_Set = new com_set; //用于设置中串口设置的调用
    ////////////////////////////////////////////////////////////////
    openglwidget *gl_widget = new openglwidget; //用于opengl界面的调用
    startprocessing *start_process; //用于开始处理类的调用
    startsavingtoharddisk *start_save_harddisk; //将数据存入硬盘的类，用于开始处理中调用的多线程
    paintonline *paint_online; //实时画图线程的类，用于将共享内存中的数据读出然后转化为画图所需的数据格式
    insdata *ins_Data; //实时抓取惯导数据包的类
    figure *start_figure; //实时计算线程的类，用于计算数据

    QThread *figure_thread; //实时计算线程，实例化在创建线程的函数中
    QThread *packet_data_thread; //抓包数据的线程，实例化在创建线程的函数中
    QThread *harddisk_thread; //存入硬盘的线程，实例化在创建线程的函数中
    QThread *area_calculation_thread; //区域插值计算的线程，实例化在创建线程的函数中
    QThread *paint_online_thread; //实时画图的线程，实例化在创建线程的函数中
    QThread *ins_data_thread; //实时读取惯导数据的线程，实例化在创建线程的函数中
    QThread *wavedisplay_thread; //实时进行波形显示的线程
    //QVector<QByteArray>*buffer_online_catch; //用于抓包线程的全局容器
    //QVector<QByteArray>*buffer_online_save; //用于存入硬盘线程的全局容器
    //QVector<QByteArray>*buffer_online_datashow; //用于显示子线程的全局容器
    //QVector<unsigned __int16>*buffer_show_processed; //定义全局容器，用于显示子线程和显示主线程中交换使用，此容器为子线程写入数据使用
    //QVector<unsigned __int16>*buffer_show_processed_mainthread; //定义全局容器，用于显示子线程和显示主线程中交换使用，此容器为主线程读数用
    //采集线程的共享内存和读写锁
    uchar *share_buffer; //用于雷达数据采集中的共享内存
    float *angleArray; //用于雷达数据采集中的角度值内存
    QReadWriteLock *share_lock; //用于雷达数据采集的共享内存的读写锁
    //惯导数据采集的共享内存和互斥锁
    double *ins_buffer; //用于惯导数据读取中的共享内存
    QMutex *mutex;
    double *figurex_buffer;
    //计算的共享内存和互斥锁
    double *figurez_buffer; //用于计算的共享内存
    QMutex *figure_mutex;
    //存储的共享内存和互斥锁
    uchar *save_buffer; //用于存储数据中的共享内存
    float *saveAngleBuffer; //用于存储线程的角度
    QReadWriteLock  *save_mutex;
    playback_readdata *pb_data; //回放部分读取数据的子线程
    QThread *playback_readfile_thread;  //回放功能中读取数据的子线程


};
#endif // MAINWINDOW_H
