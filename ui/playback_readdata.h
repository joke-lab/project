#ifndef PLAYBACK_READDATA_H
#define PLAYBACK_READDATA_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QVector>
#include <QLibrary>
#include <global.h>
#include <math.h>
#include <QDebug>
#include <QVector>
#include <commonfunction.h>

extern QVector<unsigned __int16>buffer_read; //将buffer声明为全局变量，在读取数据的子线程和主线程中同时调用
extern QVector<unsigned __int16>buffer_show;

extern QList<double>figure_read_data;
extern QList<double>figure_calculate_data;

extern int radial_start_para; //计算区域在一个角度上，800个点中，起始的点数
extern int radial_end_para; //计算区域在一个角度上，800个点中，截止的点数
extern int angle_start; //计算区域的起始角度
extern int angle_end; //计算区域的结束角度
extern int count_frame; //用于计数，当前是第几帧数据

class playback_readdata : public QObject
{
    Q_OBJECT
public:
    playback_readdata(QObject* parent = nullptr);
    ~playback_readdata();

public:
    void startread_thread();
    //void CoCIRFun(int rrawCountLine); //反异步干扰函数

signals:
    void started_readdata_message();
    void start_showing(); //用于通知playback打开绘制的全局变量
    void start_figure(); //用于通知playback打开计算的全局变量
    void pingtan_change_distance(); //发送给回放界面，再传给主界面用于改变观测距离的信号
    void longkou_sampling_step(); //龙口数据格式：发送给主界面用于更新采样步长
    void pb_paint(qint16 *buf); //龙口：向opengl发送的信号，带着数据
    void rraw_paint(int, qint16 *); //RRAW:向Opengl发送的信号，带着数据
    void rrawInsShow(double,double,double,double); //向主界面发送的显示INS的函数
    void pt_paint(int aa, int bb, int cc, int dd, qint16 *buf); //平潭：向opengl发送的信号，带着数据
    void st_ws(int, int, qint16 *); //向波形显示发送的信号，带着数据

    //计算部分的信号
    void playback_figure_message(); //向playback发送信号以表明此线程成功开启
    void playback_figure_returnvalue(QString,double,double,double,double,double,double,double,double *,double *,double *,double *); //向playback中发送信号，以在主界面上显示计算结果

private:
    int radial_start; //计算区域的起始半径
    int radial_end; //计算区域的结束半径
    QString filename_text; //从ini文件中读取的文件名
    QByteArray data; //从文件中直接读取的十六进制数据
    QByteArray angle; //从文件中直接取出的坐标（十六进制）
    qint16 decAngle = 0; //将坐标转为十进制的坐标存储
    qint16 decAngle_last = 0; //用来保存上一次读取的数据线的角度值
    qint16 decAngle_tem_storage = 0; //当一个角度的线数不够10根时，用来暂存下一个角度

    int lk_c = 0; //在开始回放中使用，用于判断是否是文件的第一根线
    int angle_count = 0; //用于记录此次读入当前向buffer中已经读入的线数
    qint16 count_to_ten = 0; //用于记录一个角度是否有10根线
    bool first_run = true; //初次运行的判断变量
    QFile file1; //设置要读取的文件
    //bool longkou_first_time_swap = true; //第一次交换内存时的信号，用来通知绘制的主线程可以开始绘制
    QString data_format; //用于存储当前选定的数据格式
    bool ok = true;
    QDataStream streamIn; //将数据从设备中读出
    bool longkou_firsttime_swap = true;
    bool lk_fl = true; //longkou-firstline
    qint16 lk_buffer[802*600];
    qint16 decRadial[800]; //将一根线的数据一次性读入的数组

    //反异步干扰
    float fGate; //门限设置
    int nGrade;
    int fGateStep;
    bool state; //状态变量设置

    //平潭数据格式下的变量
    int pingtan_count_one_frame = 0; //一帧中的线数
    int sample_per_line_int = 0; //每条线的采样数的整型
    int sample_start_distance_int = 0; //采样起始距离的整数形式
    int sample_end_distance_int = 0; //采样结束距离的整数形式
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
    qint16 *tempData; //反异步干扰处理的中间变量数组
    qint16 *tempData1;
    qint16 *tempData2;
    qint16 *RRAW_buffer; //存储一帧数据
    qint16 *cal_buffer; //存储选择区域的数据
    //int cal_angle = 0; //用于选择区域的变量
    //QVector<QVector<qint16>>RRAW_buffer; //实例化存储一帧数据二维数组（用于计算）
    //QVector<qint16>decDatatemp; //为数据设置的存储一根线的数据的数组

    bool pingtan_firsttime_swap = true; //平潭数据第一次交换时的变量
    qint16 *pt_buffer;
    int pt_angle = 0;
    int pt_count_forten = 0;
    int pt_frame = 0; //用于平潭数据中记录当前是几帧数据
    qint16 *rraw_buffer; //用于RRAW数据格式回放时存储数据的数组
    int rrawCountLine = 0; //用于记录当前是当前帧的第几根线(用于OpenGL显示画面)
    //int rrawforTen = 0; //RRAW格式下一个角度的线数计数
    int rrawAngle = 0; //RRAW格式下的角度值
    int rrawFrame = 0; //RRAW格式下的帧数
    bool rrawFirstSwap = true; //RRAW格式下第一次读取60度角度
    int rrawAngleCount = 0; //角度计数器（用于计算）
    int rrawInitialAngle = 0; //初始角度

    //台湾海峡数据格式下的变量
    unsigned __int16 taiwan_decData[480]; //为台湾海峡数据格式设置的存储一根线的数据的数组
    int taiwan_count_total_linenumber = 0; //用于判断是否是整个文件的第一根数据线
    unsigned __int16 taiwan_decAngle = 0; //将坐标转为十进制的坐标存储
    unsigned __int16 taiwan_decAngle_last = 0; //用来存储上一个角度值的变量
    int taiwan_angle_count = 0; //用于记录此次读入当前向buffer中已经读入的线数
    unsigned __int16 taiwan_count_to_ten = 0; //用于记录一个角度是否有10根线
    bool tw_firsttime_swap = true; //第一次交换内存时的信号，用来通知绘制的主线程可以开始绘制

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
};

#endif // PLAYBACK_READDATA_H
