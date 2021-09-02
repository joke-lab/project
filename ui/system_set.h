#ifndef SYSTEM_SET_H
#define SYSTEM_SET_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSettings>
#include <QGroupBox>
#include <QRadioButton>
#include <QComboBox>
#include <QStringList>
#include <QMessageBox>
#include <QDebug>
#include <global.h>
namespace Ui {
class system_set;
}

class system_set : public QWidget
{
    Q_OBJECT

public:
    explicit system_set(QWidget *parent = nullptr);
    ~system_set();

signals:
    void update_radarinform(); //发往主界面，改变采样起始结束距离和采样步长

private slots:
    void confirmSlot(); //确认按钮的槽函数
    void cancelSlot(); //取消按钮的槽函数

private:
    Ui::system_set *ui;
    void ini_set(); //站点信息的设置函数
};

#endif // SYSTEM_SET_H
