#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QDebug>
#include <QVector>
#include <QElapsedTimer>
#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QMouseEvent>
#include <QTimer>
#include <QReadWriteLock>
#include <QList>
#include <windows.h>
#include <global.h>
#include <math.h>
#include <playback_readdata.h>
#include <QGridLayout>
#include <QLabel>

extern qint16 pb_buf[802*3600];
extern qint16 *pt_buf;


namespace Ui {
class openglwidget;
}

class openglwidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit openglwidget(QWidget *parent = nullptr);
        ~openglwidget();

    void longkou_display(); //龙口数据的回放格式
    void pingtan_display(); //平潭数据的回放格式
    void taiwan_display(); //台湾海峡数据的回放格式
    void start_display(); //开始处理模式下的显示函数
    void rraw_display(); //RRAW数据的回放格式
    void anglePaint();
    void area_choose();
    void mousePressEvent(QMouseEvent *event); //计算区域模块的鼠标事件响应函数
    //uchar *ucharo;
    //QReadWriteLock *locko;
    //paintonline *paint_online = new paintonline(ucharo,locko,nullptr);

public slots:
    void receive_test_12252008(qint16 *bb,int);
    void receive_pb_paint(qint16 *buf); //接收来自回放的龙口数据
    void receive_pt_paint(int aa, int bb, int cc, int dd, qint16 *buf); //接收来自回放的平潭数据
    void receive_rraw_paint(int,qint16 *); //接收来自回放的RRAW数据

protected:
    void initializeGL(); //界面初始化函数
    void paintGL(); //绘制函数
    void resizeGL(); //大小设定函数
    //void mouseMoveEvent(QMouseEvent *event);

signals:
    void change_current_orientation(float); //发送给主界面以改变界面上的“当前方位”的值
    void change_current_frame(); //发送给主界面以改变界面上的“当前帧数”的值
    void online_update();

private:

    Ui::openglwidget *ui;

    int N = 10;
    int color[16][3]; //存储颜色的矩阵
    int maxStrength = 0; //此时可观测的最大强度值
    int miniStrength = 0; //此时可观测的最小强度值
    float anglePoint = 0; //角度信息
    float radialPoint = 0; //半径信息
    float count_ten_number = 0; //此角度下第几根线的信息
    int intensity = 0; //强度信息
    QString data_format; //回放时选择的数据格式
    int judge = 0; //将强度值处理后用来与颜色配对
    int correctAngle = 0; //用于显示的修正角度

    //计算区域选择框中的变量
    int strength_max = 0;
    int strength_min = 0;
    int radio1 = 0; //最小半径
    int radio2 = 0;  //最大半径
    int angle1 = 0; //起始角度
    int angle2 = 0; //结束角度
    int n1_1_left = 0; //算出弧线（小半径，0度左边）所需的点数
    int n1_2_left = 0; //算出弧线（大半径，0度左边）所需的点数
    int n1_1_right = 0; //算出弧线（小半径，0度右边）所需的点数
    int n1_2_right = 0; //算出弧线（大半径，0度右边）所需的点数
    int n_2 = 0; //算出两条直线所需的点数
    bool zero_cross = false; //用于判断区域是否跨零
    void region_choose_display(); //绘制计算区域选择框的函数

    const char *str2 = "30";
    const char *str3 = "60";
    const char *str4 = "90";
    const char *str5 = "120";
    const char *str6 = "150";
    const char *str7 = "180";
    const char *str8 = "210";
    const char *str9 = "240";
    const char *str10 = "270";
    const char *str11 = "300";
    const char *str12 = "330";
    const char *str1 = "360";

    int c = 0; //对于龙口60度一次刷新，用于计数，现在是第几次
    int cRRAW = 0; //对于RRAW60度一次刷新用于计数
    float pi = 3.1415926536;//Π值
    GLuint list_coord_axis; //坐标轴的显示列表
    GLuint list_angle_build; //角度值的显示列表
    int pingtan_line_one_frame = 0; //平潭：一帧中的线数
    int pingtan_sample_per_line = 0; //平潭：每条线采样数
    int pingtan_sample_start_distance = 0; //平潭：采样起始距离
    int pingtan_sample_end_distance = 0; //平潭：采样结束距离
    float pingtan_sampling_step_size = 0; //平潭：采样步长
    int pingtan_now_countline = 0; //用来记录当前h绘制的是第几根线
    float pingtan_angle = 0; //用来记录当前的角度
    int pingtan_intensity; //平潭：强度信息
    bool pt_first = true;
    int pt = 0; //对于平潭60度一次刷新，用于计数，现在是第几次
    qint16 *rrawBuf; //用于存储rraw的数组
    int rrawPerPoint = 0; //用于记录RRAW格式回放下一根数据线上的点数
    float rrawAngle = 0; //RRAW格式回放下的角度值
    float rrawInten = 0; //用于记录每个强度值点

    //“开始处理”部分的变量
    int count_vector = 1; //用于计数，判断是否将一帧的数据存储满了
    float start_angle = 0; //暂定，初始角度为0
    int start_strength = 0;
    int point_num = 0;

    qint16 *buffer_opengl_start_show = new qint16[3600*2001]; //在开始处理时，在此容器中存入一帧的数据用以显示
    QFile file;

    bool first_online = 1;
    int countOnline = 0; //临时计数
};

#endif // OPENGLWIDGET_H
