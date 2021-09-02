#ifndef PAINTONLINE_H
#define PAINTONLINE_H

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <QVector>
#include <QReadWriteLock>
#include <QThread>
#include <QDebug>
#include <QSettings>
#include <QLibrary>
#include <QCoreApplication>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QSerialPort>
#include <QElapsedTimer>
#include <QReadWriteLock>
#include <global.h>
#include <math.h>
#include <commonfunction.h>
extern QVector<double>a_buffer;
extern QVector<double>d_buffer;

class paintonline : public QObject
{
    Q_OBJECT
public:
    paintonline(uchar*&,float *&,QReadWriteLock *,double *&, QMutex *,double *&,double *&,QMutex *,uchar *&,float *&,QReadWriteLock *,QObject* parent = nullptr);
    ~paintonline();

public:
    uchar *buf_read;
    float *angle_Buffer;
    QReadWriteLock *lock_read;
    double *insData;
    QMutex *insMutex;
    double *figurexData;
    double *figurezData;
    QMutex *figureMutex;
    uchar *saveData;
    float *saveAngle;
    QReadWriteLock *saveMutex;
    double *figurexTemp;
    double *figurezTemp;
    //openglWidget *opengl = new openglWidget;
    int test_count = 0; //每运行一次加一，角度增加10度

public slots:
    void paintonline_thread(); //实时画图坐标的读取函数
    void paintonline_thread2(int); //循环体中的函数

signals:
    void paint_online_message(); //在主线程中捕捉的信号，用来判断此线程是否运行正常
    void start_showing_online(); //发送信号回主界面，开启显示主线程中的变量
    void test_19252008(qint16 *,int); //向opengl主线程发送信号，用以更新局部的数据

    void frame(int); //将当前帧数发送给主界面的函数
    void naviValue(int,int,double,double);
    void save_harddisk_thread(double *); //发送信号给存入硬盘的线程，将数据存入文件中
    void figurethread(double *); //发送信号给计算线程
    void gps_toheadSave(int *); //发送经纬度至数据保存线程，存入数据文件头中
    void st_wave(int, int, qint16 *); //发送至波形显示


private:
    int N = 10;
    bool state; //状态变量设置
    int flag_bit1,flag_bit2,flag_bit3,flag_bit4; //每个数据组开头的标志位，0x0f
    int message_sequence_identifier; //消息序列标识符，用以检测上下数据包是否是连续的
    double angle_start; //一根线中的起始角度
    double angle_end; //一根线中的结束角度
    QByteArray data_array; //用来暂存从共享容器中读出的一个数据组
    double startradial; //计算区域的起始半径
    double endradial; //计算区域的截止半径
    double startangle; //计算区域的开始角度
    double endangle; //计算区域的截止角度
    int start_count_radial; //计算区域内一个角度上起始半径对应的点是第几个（中心为零）
    int end_count_radial; //计算区域内一个角度上结束半径对应的点是第几个（中心为零）
    bool isBoard = 0; //用于判断是否是船载
    bool isFirst = true;
    double angleLast = 0;
    int testCountOld = 0;

    int angle_old = 0; //线数计数
    int count = 0;
    bool first = true;
    int angle_cor = 0; //雷达修正角度
    int angle = 0; //修正之后传给opengl的角度值，比正确值大10倍
    int nx = 0;
    int ny = 0;
    int nt = 0;
    double *x;
    double *y;
    double *z;
    double begin = 0;
    double end = 0;
    //int data_count = 0; //与playback_readdata之间共享内存的长度
    int count_line = 0;
    int num_paint = 0; //通过观测范围和采样间距计算出采样点数

    QSerialPort *serial1; //惯导输入的串口
    QSerialPort *serial2; //输出的串口
    QByteArray gpsData; //从惯导读出来的数据
    QString gpsData_s;
    int gpsHead; //惯导模块的第一个帧头位置
    QVector<int>gpsHeadAll;
    int gpsLength; //一个数据包的长度
    QByteArray gpsDataPack,gpsDataBUnInt;
    unsigned int gpsDataUnInt[2]; //用于存储“导航时间”和“用于分包发送附加结构体”
    QByteArray gpsDataBFloat; //用于存储gps坐标中10个float参数
    float gpsDataFloat[10],gpsDataFloatRYP[3]; //用于存储NAVITEST协议中连续10个float变量的数组；用于存储协议中滚动、航向和俯仰三个值
    int gpsDataInt[2]; //用于存储经纬度的数组
    unsigned int gpsDataUnIntTime; //用于存储unsigned int类型UTC时间的变量
    unsigned short gpsDataUnShortTime; //用于存储unsigned short类型UTC时间的变量
    double gpsDataFloatTime; //UTC时间结果的存储变量
    qint64 gpsDataIntTime; //UTC时间结果的最终存储变量
    short gpsDataHeave; //用于存储short类型升沉位移
    bool ins = false; //用于判断惯导是否有输入
    bool out = false; //用于判断是否需要对外输出计算结果
    int frameTotal = 0; //一组数包含的帧数，在设置中自定义

    double insTemp[81]; //用于存储惯导数据的数组
    int insNum = 0; //用于记录共享内存中的惯导组数
};


#endif // PAINTONLINE_H
