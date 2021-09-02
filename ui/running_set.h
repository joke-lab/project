#ifndef RUNNING_SET_H
#define RUNNING_SET_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QString>
#include <QObject>
#include <QButtonGroup>
#include <QDir>
#include <QFileDialog>
#include <QDebug>

#include "pcap.h"
#include <stdlib.h>
#include <stdio.h>
namespace Ui {
class running_set;
}

class running_set : public QWidget
{
    Q_OBJECT

public:
    explicit running_set(QWidget *parent = nullptr);
    ~running_set();
    QButtonGroup *buttongroup; //采集方式中的三个选择按钮
signals:
    void runset_send_to_loginformation(); //向主界面类中发送信号以刷新系统信息栏
private slots:
    void saveSlot(); //保存按钮的槽函数
    void cancelSlot(); //取消按钮的槽函数
    void radiobuttonSlot(); //采集方式三个radiobutton的槽函数
    void checkbox_state_ChangedSlot(); //自动存储原始数据的勾选框状态改变的槽函数
    void savepathSlot();  //存储路径处按钮的槽函数
    void cal_res_savepathSlot();  //计算结果的存储路径处按钮的槽函数

private:
    Ui::running_set *ui;
    void NICname_read(); //网卡读取函数
    void save_Path(); //存储路径和计算结果存储路径的函数
    void collection_mode(); //采集方式模块的函数
};

#endif // RUNNING_SET_H
