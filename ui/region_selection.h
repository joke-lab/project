#ifndef REGION_SELECTION_H
#define REGION_SELECTION_H

#include <QWidget>
#include <QDialog>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <region.h>
namespace Ui {
class region_selection;
}

class region_selection : public QWidget
{
    Q_OBJECT

public:
    explicit region_selection(QWidget *parent = nullptr);
    ~region_selection();

signals:
    void area_change_Slot(); //发送给主界面，以改变主界面上的可观测范围的信息

private:
    Ui::region_selection *ui;
    region *regionchoose = new region;
    QButtonGroup *buttongroup = new QButtonGroup; //将两个radiobutton设置为group以检测其id号
    void ini_initial(); //当打开界面时，用于初始化界面控件状态和值的函数

private slots:
    void saveSlot(); //保存按钮对应的槽函数
    void cancelSlot(); //取消按钮对应的槽函数
    void radiobuttonSlot(); //当改变radiobutton的状态时所对应的槽函数
    void addSlot();
    void removeSlot();

};

#endif // REGION_SELECTION_H
