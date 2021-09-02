#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.setWindowFlags(Qt::WindowCloseButtonHint|Qt::WindowMinimizeButtonHint|Qt::FramelessWindowHint); //设置主窗口退出、最小化、最大化选择图标取消
    w.setAttribute(Qt::WA_QuitOnClose,true);
    QFile qss(":MacOS.qss");
    if(qss.open(QFile::ReadOnly))
    {
        qDebug() << "样式表打开成功";
        QString style = qss.readAll();
        a.setStyleSheet(style);
        qss.close();
    }
    else
    {
        qDebug() << "样式表打开失败";
    }

    //检测当前qt所有的数据库驱动
    QStringList drivers = QSqlDatabase::drivers();
    foreach(QString driver,drivers)
        qDebug()<<driver;
    w.show();
    return a.exec();
}
