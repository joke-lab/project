#ifndef STARTSAVINGTOHARDDISK_H
#define STARTSAVINGTOHARDDISK_H

#include <QObject>
#include <QSharedMemory>
#include <QString>
#include <QByteArray>
#include <QThread>
#include <QSettings>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDataStream>
#include <QIODevice>
#include <QBuffer>
#include <QReadWriteLock>
#include <QMutex>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>

#include <global.h>

class startsavingtoharddisk : public QObject
{
    Q_OBJECT
public:
    startsavingtoharddisk(uchar *&,float *&,QReadWriteLock *,QObject* parent = nullptr);
    ~startsavingtoharddisk();

public:
    void harddisk_save_thread(); //将数据存入硬盘的线程
    void filehead_Slot(); //存储每个数据文件头
    uchar *buf_read;
    float *angle_buf;
    QReadWriteLock *lock_read;

public slots:
    void harddisk_save_thread2(double *); //循环体中的内容
    void gps_toheadSave_Slot(int *);

signals:
    void start_saving_message(); //在主线程中捕捉的信号，用来判断此线程运行是否正常

private:
    int N = 10;
    float saveAngle_old = 0;
    //QByteArray array; //用来暂存的数组
    bool for_first_run = true; //用于判断是否是第一次运行
    int count_for_frame = 0; //用于记录数据包的个数是否满六十四帧
    QFile file; //要打开的文件的声明
    QDataStream streamIn;
    QString filename; //要存入的文件的名称
    QByteArray insData_save;
    int sample_num = 0; //通过观测范围和采样间距计算出采样点数
    int gps_headData[2] = {0};
    int min = 0; //可观测范围最小值
    int max = 0; //可观测范围最大值
    int frame_num = 0; //每个数据文件存储的原始数据帧数
    double insData[16*200]; //用于存储paintonline传输的惯导数据
    char markerBit = 0x01; //存入数据文件的标志位
    //QByteArray keyword;
    //QByteArray pos_first;
    //QByteArray pos_last;
};

#endif // STARTSAVINGTOHARDDISK_H
