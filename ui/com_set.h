#ifndef COM_SET_H
#define COM_SET_H

#include <QWidget>
#include <QDialog>
#include <QCheckBox>
#include <QStringList>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>
#include <QObject>
#include <QDebug>
namespace Ui {
class com_set;
}

class com_set : public QWidget
{
    Q_OBJECT

public:
    explicit com_set(QWidget *parent = nullptr);
    ~com_set();

private:
    Ui::com_set *ui;

private slots:
    void save_Slot(); //保存按钮对应的槽函数
    void cancel_Slot(); //取消按钮对应的槽函数
};

#endif // COM_SET_H
