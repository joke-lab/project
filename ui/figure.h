#ifndef FIGURE_H
#define FIGURE_H

#include <QObject>
#include <QReadWriteLock>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include <paintonline.h>
#include <global.h>
#include <stdio.h>

class figure : public QObject
{
    Q_OBJECT
public:
    figure(double *&,double *&,QMutex *,QObject* parent = nullptr);
    ~figure();

public:
    double *bufx;
    double *bufz;
    QMutex *lock_read;
    double sample[23]; //用于存放一系列参数，用于dll中函数的输入
    void figure_thread1();

public slots:
    void figure_thread2(double *);

signals:
    void result(QString,double,double,double,double,double *,double *,double *,double *); //将计算结果发送给主界面的函数
    void databaseError(); //数据库错误发送信号至主线程

private:
    int nx = 0;
    int ny = 0;
    int nt = 0;
    double *x;
    double *z;
    double *y;
    double *dataOut; //计算直角坐标系的函数的输出数组
    double *dataResult; //反演结果的输出数组
    double angle_start; //一根线中的起始角度
    double angle_end; //一根线中的结束角度
    double startradial; //计算区域的起始半径
    double endradial; //计算区域的截止半径
    double startangle; //计算区域的开始角度
    double endangle; //计算区域的截止角度
    double start_count_radial; //计算区域内一个角度上起始半径对应的点是第几个（中心为零）
    double end_count_radial; //计算区域内一个角度上结束半径对应的点是第几个（中心为零）
    bool dll_isload = false; //用于标志动态链接库是否加载成功
    int num_paint = 0; //通过观测范围和采样间距计算出采样点数
    //定义函数头，为动态链接库中，将所选的极坐标区域变为直角坐标的函数
    typedef void(_stdcall *QTinterFun)(double*,double*,double*,double*,int ,int ,int ,double*,double,double,int);
    //定义函数头，为动态链接库中，计算海态参数的函数
    typedef void(_stdcall *QTinversCalculate)(double*,double*,double,double,double*);
    QLibrary dll; //动态链接库的对象
    QTinterFun interfun;
    QTinversCalculate invers_calculation;
    int out; //判断输出串口是否打开
    QSerialPort *serial1; //输出串口
    QSqlDatabase db; //数据库定义
    //QSqlQuery query; //创建表定义
    QString create_sql; //创建表格定义
    QString name = nullptr; //表名定义
    QDateTime time; //系统时间
    QString year_t = nullptr; //系统时间年
    QString month_t = nullptr; //系统时间月
    QString day_t = nullptr; //系统时间天
    QString hour_t = nullptr; //系统时间小时
    QString minute_t = nullptr; //系统时间分

    QStringList dataList; //数据类型列表
    QSqlQuery queryAdd; //创建新表定义
    QString add;
    QString judgeTime; //日期判别定义
    QString set_time; //读取数据库上一次存储时间（年月日）
};

#endif // FIGURE_H
