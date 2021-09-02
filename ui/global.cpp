#include "global.h"

bool global::startreview_clicked = false; //当点击“开始回放”按钮时，变为true
bool global::run_firsttime = false; //用于数据回放第一次时判断buffer是否读入
bool global::area_display = false; //区域显示的全局函数
bool global::area_hiding = false; //区域隐藏的全局函数
bool global::start_or_stop_dealing = true; //下拉菜单中开始处理按钮，开始处理或停止处理状态的全局函数
bool global::start_running_thread = false; //用于多线程判断是否开始运行的标志
bool global::bool_catch = true; //用于抓包线程中向共享内存中存入数据后改变的值，用于开启存储、计算、显示三个线程对共享内存的操作
bool global::bool_save = false; //用于向硬盘中存储数据线程对共享内存操作完毕后改变的值，用于通知抓包线程可以继续向共享内存中存入数据
bool global::bool_datashow = false; //用于显示子线程中，将数据全局处理完毕后改变的值,用于通知线程可以继续向其共享容器中存入数据
bool global::playback_thread_stop = false; //用于在主线程中控制数据回放的读取数据子线程是否关闭的变量
bool global::playback_mutithread_read = true; //用于在回放多线程中控制读取的子线程开始读取的变量
bool global::playback_mutithread_show= false; //用于在回放多线程中控制显示的主线程开始显示的变量
bool global::playback_mutithread_figure_readdata = true;
bool global::playback_mutithread_figure_calculatedata = false;
bool global::first_time_figure_readdata = true;
bool global::taiwan_first_time_swap = true; //台湾海峡数据类型：点击开始回放按钮时设置此变量为真
bool global::start_play_online = false; //开始处理时，开启opengl中绘图的变量
bool global::start_play_first_swap = true; //开始处理时，用于判断显示的子线程和显示的主线程的第一次交换数据
bool global::start_show_mutithread_dataread = true; //开始处理时，用于控制显示子线程和显示主线程间共享内存操作的处理，防止两个线程同时对共享内存进行操作
bool global::start_show_mutithread_datashow = false; //开始处理的opengl主线程：开始处理时，用于控制显示子线程和显示主线程间共享内存操作的处理，防止两个线程同时对共享内存进行操作
bool global::playback_lock_openglshow = true;
bool global::ins_start = true; //用于对惯导数据读取的控制
bool global::transceiver_turnon_bool = false; //初始情况下发射机处于关闭状态
bool global::power_radar_bool = false; //初始情况下雷达处于关闭状态
bool global::rrawFirst = true; //rraw数据文件回放时opengl中初始化数组的变量
bool global::waveshow = false; //设置波形显示开关默认为关
//QSettings ini(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
//bool global::state = ini.value("/radar_information/state").toBool(); //判断是否需要反异步干扰的状态
//QByteArray global::insdataAll[]; //存储全部惯导数据
//QVector<unsigned __int16> global::sampleAll; //存储所有sample数据
