#ifndef STARTPROCESSING_H
#define STARTPROCESSING_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QCoreApplication>
#include <QByteArray>
#include <QThread>
#include <QChar>
#include <QVector>
#include <QReadWriteLock>
#include <global.h>
#include "pcap.h"
#include <stdio.h>
#include <math.h>
#include <QDebug>
#include <QTimer>

class startprocessing :public QObject
{
    Q_OBJECT
public:
    startprocessing(u_char *&,float *&,QReadWriteLock *,QObject* parent = nullptr);
    ~startprocessing();

public:
    void packet_grab_thread(); //抓取数据包的线程
    void packet_grab_thread2();
    uchar *buf_write;
    float *angleArray;
    QReadWriteLock *lock_write;
    //uchar *buf_cap = new uchar[100*1280]; //在抓包子线程中用于暂存数据的数组832
    //paintonline *paint_online;
    //startSavingtoharddisk *save;

signals:
    void start_process_message(); //在主线程中捕捉的信号，用来判断此线程运行是否正常
    void start_other_thread(); //在主线程中捕捉的信号，用来开启其他几个线程
    void preprocessing(int);
    void saving();

private slots:
    void net_ini();
    void datagrab();

private:
    int N = 10;
    //基本信息
    char *name; //网卡名称
    int x;
    int min;
    int max;
    int num; //采样点数
    int c_time; //采集次数
    int t_interval; //时间间隔
    int s_interval; //存储间隔
    //计时器
    QTimer *t_timer;

    pcap_t *adhandle;
    pcap_if_t *alldevs; //保存扫描结果
    struct tm *ltime;
    char timestr[16];
    time_t local_tv_sec;
    char errbuf[PCAP_ERRBUF_SIZE]; //错误信息
    bpf_u_int32 netmask; //掩码
    struct bpf_program fcode;
    int res;
    struct pcap_pkthdr * header;
    const uchar *pkt_data;

    //存储每个数据包的结构体，分为数据头和数据内容两个结构体
    typedef struct data{
        uchar head_byte_data[12];
        uchar data_byte_data[4040]; //832（北京） 4040为南京一个包大小
        //uchar last_byte_data[3];
    }data;

    data *data_point; //结构体的声明

    char *last_end = new char[2];
    int count = 0; //每计数100次，将QByteArray中的数据拷贝到共享内存中
    bool for_signal_initial = true; //用于判断是否是第一次拷贝数据进共享内存
    int angle_a = 0; //角度值的整数部分
    int angle_b = 0; //角度值的小数分母部分
    int angle_c = 0; //角度值的小数分子部分
    float angle = 0; //角度值
    float angle_old = 0; //用于存储上一个角度值
    float angle_temp = 0;

    bool first_run = true;
    uchar temp[8040]; //用于存放临时数据包的数组
    int pacJudge = 0; //用于判断一根线分为了几个数据包
    int pos = 0; //用于判断数据是一根线上的第几个
    int pacCount = 0; //用于记录数据包中的计数位的值
    int pacCountOld = 0; //用于记录上一个数据包中计数位的值
    bool isunLock = true; //用于判断是否需要上锁
    float angleCha = 0; //角度差值

};

#endif // STARTPROCESSING_H
