#ifndef INSTEST_H
#define INSTEST_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QPushButton>
#include <QGridLayout>
#include <QSettings>
#include <QMessageBox>
#include <QSerialPort>
#include <math.h>

namespace Ui {
class instest;
}

class instest : public QWidget
{
    Q_OBJECT

public:
    explicit instest(QWidget *parent = nullptr);
    ~instest();

private:
    Ui::instest *ui;
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
    bool isstop = false; //用于判断是否停止读取数据

private slots:
    void test_Slot(); //开始测试按钮的槽函数
    void stop_Slot(); //停止测试按钮的槽函数
    void exit_Slot(); //退出按钮的槽函数
};

#endif // INSTEST_H
