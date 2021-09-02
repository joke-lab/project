#ifndef COLOR_SET_H
#define COLOR_SET_H

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QMouseEvent>
#include <QObject>
#include <QEvent>
#include <QDebug>
#include <QPalette>
#include <QBrush>
#include <QColor>
#include <QColorDialog>
#include <global.h>

namespace Ui {
class color_set;
}

class color_set : public QWidget
{
    Q_OBJECT

public:
    explicit color_set(QWidget *parent = nullptr);
    ~color_set();
signals:
    void colorbar_change();

protected:
    bool eventFilter(QObject *watched, QEvent *event); //事件滤波器函数

private:
    Ui::color_set *ui;
    QLineEdit *color_edit = new QLineEdit; //颜色设置的编辑条
    QLabel *strength_label = new QLabel[17]; //用于显示强度值的18个标签
    QLabel *color_label = new QLabel[16]; //定义一个大小为17的数组，用来存放13个颜色label
    //事件过滤器中所用的变量,前缀均为eventfilter
    QPalette eventfilter_palette;
    QString eventfilter_red;
    QString eventfilter_green;
    QString eventfilter_blue;

    void colorBar(QVBoxLayout *,QVBoxLayout *); //用来显示颜色和强度值的函数
    QColorDialog *colordialog; //调色盘
    int i_change; //用于记录当前选中的颜色框的位置

private slots:
    void initial_color_slot();
    void initial2_color_slot();
    void initial3_color_slot();
    void initial4_color_slot();
    void colorDialog_slot(); //调色盘按钮的槽函数，用于打开调色盘
    void confirm_slot(); //保存按钮的槽函数
    void cancel_slot(); //取消按钮的槽函数
    void getcolorSlot(QColor); //当调色盘点击ok时，获取其选中的颜色的槽函数
};

#endif // COLOR_SET_H
