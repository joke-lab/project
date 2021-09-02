#ifndef WAVEVIEW_H
#define WAVEVIEW_H

#include <QWidget>
#include <QButtonGroup>
#include <QRadioButton>
#include <iostream>
#include <qcustomplot.h>
#include <global.h>
#include <QTimer>

namespace Ui {
class waveview;
}

class waveview : public QWidget
{
    Q_OBJECT

public:
    waveview(QWidget *parent = nullptr);
    ~waveview();
public:
    uchar *data;


private:
    Ui::waveview *ui;
    int N = 10;
    int framenum; //采样点数
    //波形显示扫描角度选择
    QButtonGroup *radioButtonGroup; //角度选择的两种方式
    QCustomPlot *p; //设置widget为指针

    bool id; //识别选择方式
    QTimer *mTimer;
    int n_angle; //当前角度
    int r_c = 0; //判断角度（回放）
    qint16 *w_buf; //存储数据
    int num ; // 存储角度信息

private slots:
    void radiobuttonSlot();
    void openSlot(int, int, qint16 *);
    void stopSlot();
    void quitSlot();
    void addDataandShow();
    void myMoveEvent(QMouseEvent *);
    //void choose(int, qint16 *);
    void save_p();
};

#endif // WAVEVIEW_H
