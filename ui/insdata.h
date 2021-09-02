#ifndef INSDATA_H
#define INSDATA_H

#include <QObject>
#include <QSerialPort>
#include <QSettings>
#include <QCoreApplication>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QDebug>
#include <QMutex>
#include <global.h>
#include <math.h>
#include <QElapsedTimer>

class insdata : public QObject
{
    Q_OBJECT
public:
    insdata(double *,QMutex *,QObject *);
    void insData_grab_thread1();
    void insData_grab_thread2();

    double *buffer; //用于存储惯导数据的共享内存
    QMutex *mutex; //用于惯导内存的锁
    ~insdata();

signals:
    void insinitialEnd(); //发送给主线程的反馈信号，表明惯导线程初始化完毕

private:
    QSerialPort *serial1; //惯导输入的串口
    int ins; //用于判断惯导输入是否打开
    QByteArray gpsData; //从惯导读出来的数据
    QString gpsData_s;
    int gpsHead; //惯导模块的第一个帧头位置
    QVector<int>gpsHeadAll;
    int gpsLength; //一个数据包的长度
    QByteArray gpsDataPack;
    int count = 0; //用于记录此次惯导数据的个数
    bool needmutexLock = true; //用于判断线程锁是否应该上锁
    bool ismutexLock = false; //用于判断线程锁当前的状态
};

#endif // INSDATA_H
