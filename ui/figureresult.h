#ifndef FIGURERESULT_H
#define FIGURERESULT_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QDateTimeEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFileDialog>
#include <QAxObject>
#include <QDesktopServices>
#include <QDir>
#include <QString>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class figureresult;
}

class figureresult : public QWidget
{
    Q_OBJECT

public:
    explicit figureresult(QWidget *parent = nullptr);
    ~figureresult();

private:
    Ui::figureresult *ui;
    QSqlDatabase db; //数据库
    int row = 0; //表的初始行数
    float *tempData; //用于存放临时数据的数组
    QVector<QString>tempTimeName; //用于存放临时数据的数组

private slots:
    void search_Slot(); //查询按钮的槽函数
    void export_Slot(); //导出按钮的槽函数
    void close_Slot(); //关闭按钮的槽函数
};

#endif // FIGURERESULT_H
