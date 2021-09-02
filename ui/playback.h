#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <QWidget>
#include <QFile>
#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QTextStream>
#include <QByteArray>
#include <QIODevice>
#include <QList>
#include <QDateTime>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QSettings>
#include <QThread>

#include <global.h>
#include <playback_readdata.h>

extern int playback_static_varying; //设置一个全局变量，用以判断当前回放界面的状态是开始回放还是停止回放还是保存并退出

namespace Ui {
class playback;
}

class playback : public QWidget
{
    Q_OBJECT

public:
    explicit playback(QWidget *parent = nullptr);
    QString filename; //打开文件的名称
    ~playback();

public slots:
    void start_showing_Slot(); //当读取线程第一次进行了内存交换后调用的槽函数

signals:
    void start_playback_readfile_thread(); //用以触发开始读取数据线程的耗时函数的信号
    void pingtan_change_distance_second(); //发送给主界面用于改变观测距离的信号
    void start_playback(); //用以发送给主界面更新系统信息的信号
    void longkou_sampling_step_transmit(); //龙口数据：用以发送给主线程更新“采样步长”
    void zeroing_frame(); //用以在点击“停止回放”时发送给主界面，将当前帧数和总帧数都置为0
    void playback_longkou_figure(double r1,double r2,double r3,double r4); //接收来自计算线程的信号时，用于向主线程中发送信号，以改变主界面上的最终计算结果

private:
    Ui::playback *ui;
    playback_readdata *read_data = new playback_readdata; //读取本地数据的类，用于数据回放功能中调用的多线程
    QThread *playback_readfile_thread;  //回放功能中读取数据的子线程
    bool first_time_start = true; //判断是否是第一次点击开始回放按钮

private slots:
    void openFileSlot(); //打开文件的槽函数
    void startReviewSlot(); //开始回放的槽函数
    void stopReviewSlot(); //停止回放的槽函数
    void exitSlot(); //退出界面的槽函数
    void start_readfile_received_Slot(); //接收来自读取数据子线程中返回信息的槽函数
    void pingtan_change_distance_Slot(); //接收从数据读取中传来的平潭数据改变观测范围的槽函数
    void receive_longkou_sampling_step_Slot(); //接收从数据读取中传来的龙口数据的采样步长的槽函数
    void receive_playback_figure_message_Slot(); //接收从计算子线程中返回的信息，表明线程已成功开启
};

#endif // PLAYBACK_H
