#ifndef GLOBAL_H
#define GLOBAL_H

#include <QVector>
#include <QSettings>
#include <QCoreApplication>

//用来存储全局变量的类
class global
{ 
public:
    static bool startreview_clicked; //用于判断是否点击了“开始回放”按钮
    static bool run_firsttime; //用于数据回放第一次时判断buffer是否读入
    static bool area_display; //用于判断区域显示的radiobutton是否被选中
    static bool area_hiding; //用于判断区域隐藏的radiobutton是否被选中
    static bool start_or_stop_dealing; //用于判断开始处理按钮是处于开始处理状态还是停止处理状态
    static bool start_running_thread; //用于对于多个线程暂停和启动状态转换的全局变量
    static bool bool_catch; //用于抓包线程中抓取完毕后改变的值
    static bool bool_save; //用于向硬盘中存储数据后改变的值
    static bool bool_datashow; //用于显示子线程中读完数据后改变的值
    static bool playback_thread_stop; //用于在主线程中控制数据回放的子线程关闭的变量
    static bool playback_mutithread_read; //用于在回放时多线程中读取数据的子线程给一个读取完毕的信号
    static bool playback_mutithread_show; //用于在回放时多线程中显示数据的主线程给一个绘制完毕的信号
    static bool playback_mutithread_figure_readdata;
    static bool playback_mutithread_figure_calculatedata;
    static bool first_time_figure_readdata;
    static bool taiwan_first_time_swap; //台湾海峡数据类型：点击开始回放按钮时设置此变量为真
    static bool start_play_online; //开始处理时，开启opengl中绘图的变量
    static bool start_play_first_swap; //开始处理时，用于判断显示的子线程和显示的主线程的第一次交换数据
    static bool start_show_mutithread_dataread; //开始处理的显示子线程：开始处理时，用于控制显示子线程和显示主线程间共享内存操作的处理，防止两个线程同时对共享内存进行操作
    static bool start_show_mutithread_datashow; //开始处理的opengl主线程：开始处理时，用于控制显示子线程和显示主线程间共享内存操作的处理，防止两个线程同时对共享内存进行操作
    static bool playback_lock_openglshow;
    static bool ins_start; //用于对惯导数据读取的控制
    static bool  transceiver_turnon_bool; //当发射机打开时，此变量为真
    static bool  power_radar_bool; //当雷达打开时，此变量为真
    static bool rrawFirst;
    static bool state; //反异步状态
    static bool waveshow; //波形显示

    //存储所有sample数据
    //static QVector<unsigned __int16> sampleAll;

    /*
    typedef struct sampleAll{
        uchar head_byte_data[];
        uchar data_byte_data[];
        //uchar last_byte_data[3];
    }sampleAll;
    static QByteArray insdataAll[3600]; //存储全部惯导数据
    */
};

#endif // GLOBAL_H
