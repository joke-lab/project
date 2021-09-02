#ifndef FILESDEAL_H
#define FILESDEAL_H

#include <QObject>
#include <QFile>
#include <QByteArray>
#include <QSettings>
#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QLibrary>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <math.h>
#include <QDebug>

class filesdeal : public QObject
{
    Q_OBJECT
public:
    filesdeal();
    ~filesdeal();

public slots:
    void filesInitial_Slot(); //计算前的初始化槽函数
    //void lkDeal_Slot(int,QString,QString); //龙口格式下的处理槽函数
    //void ptDeal_Slot(int,QString,QString); //平潭格式下的处理槽函数
    void rrawDeal_Slot(int,QString,QString,QSqlQuery *); //RRAW格式下的处理槽函数

signals:
    void filesResult(int,double,double,double *,QString);

private:
    QFile file; //要打开开始进行处理的目标文件
    double *excelValue; //存储用于传输至主线程存储到excel的值
    int frame; //当前帧数
    QDataStream streamIn;
    QSettings *ini;
    //龙口
    QByteArray data; //从文件中直接读取的十六进制数据
    QByteArray angle; //从文件中直接取出的坐标（十六进制）
    qint16 decAngle = 0; //将坐标转为十进制的坐标存储
    qint16 decAngle_last = 0; //用来保存上一次读取的数据线的角度值
    qint16 decAngle_tem_storage = 0; //当一个角度的线数不够10根时，用来暂存下一个角度
    bool ok = true;
    bool lk_fl = true; //longkou-firstline
    qint16 decRadial[800]; //将一根线的数据一次性读入的数组
    int angle_count = 0; //用于记录此次读入当前向buffer中已经读入的线数
    qint16 count_to_ten = 0; //用于记录一个角度是否有10根线
    int angle_start; //计算区域的起始角度
    int angle_end; //计算区域的结束角度
    int radial_start; //计算区域的起始半径
    int radial_end; //计算区域的结束半径
    int startP = 0; //计算区域在一个角度上，800个点中，起始的点数
    int endP = 0; //计算区域在一个角度上，800个点中，截止的点数

    QList<double>list; //存放需计算的数据

    //平潭数据格式下的变量
    int pingtan_count_one_frame = 0; //一帧中的线数
    int sample_per_line_int = 0; //每条线的采样数的整型
    int ssDis = 0; //采样起始距离的整数形式sample_start_Distant
    int seDis = 0; //采样结束距离的整数形式sample_end_Distant
    int sample_frames_int = 0; //采样帧数的整数形式
    QByteArray angular_resolution; //角度分辨率
    int angular_resolution_int; //角度分辨率的十六进制整数形式
    float angular_resolution_float; //角度分辨率的浮点数形式
    QByteArray sample_internal; //采样间隔
    int sample_internal_int; //采样间隔的整数形式
    float sample_internal_float; //采样间隔的浮点数形式
    //unsigned __int16 *decData = new unsigned __int16[sample_per_line_int]; //将一根线的数据一次性读入的数据
    int pingtan_count_line = 0; //用于记录当前是当前帧的第几根线
    bool pingtan_first_run = true; //用于判断是否是第一次运行平潭数据
    //bool pingtan_first_time_swap = true; //判断是否是第一次交换内存数据
    qint16 *decData; //为平潭数据设置的存储一根线的数据的数组
    bool pingtan_firsttime_swap = true; //平潭数据第一次交换时的变量
    qint16 *pt_buffer;
    int pt_angle = 0;
    int pt_count_forten = 0;
    int pt_frame = 0; //用于平潭数据中记录当前是几帧数据

    //计算部分的变量
    double sample[23]; //用于存放一系列参数，用于dll中函数的输入
    int nx = 0;
    int ny = 0;
    int nt = 0;
    double *x;
    double *y;
    double *z;
    double *dataOut; //计算直角坐标系的函数的输出数组
    double *dataResult; //反演结果的输出数组
    double begin = 0;
    double end = 0;
    //int data_count = 0; //与playback_readdata之间共享内存的长度
    int count_line = 0;
    bool dll_isload = false; //用于标志动态链接库是否加载成功

    //定义函数头，为动态链接库中，将所选的极坐标区域变为直角坐标的函数
    typedef void(_stdcall *QTinterFun)(double*,double*,double*,double*,int ,int ,int ,double*,double,double,int);
    //定义函数头，为动态链接库中，计算海态参数的函数
    typedef void(_stdcall *QTinversCalculate)(double*,double*,double,double,double*);
    QLibrary dll; //动态链接库的对象
    QTinterFun interfun;
    QTinversCalculate invers_calculation;

    int check = 0; //用于判断是否需要输出txt
    QString txtPath; //存储txt文件的路径
    int rrawFrame = 0; //RRAW数据格式中的帧数计数器
    int rrawCountLine = 0; //RRAW数据格式中的线数计数器
    int rrawAngle = 0; //RRAW数据格式中的角度值
    double longitude = 0; //经度
    double latitude = 0; //纬度

};

#endif // FILESDEAL_H
