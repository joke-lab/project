#include "openglwidget.h"
#include "ui_openglwidget.h"
#include <QTime>
#include <QBitmap>
#include <QDebug>

/*
//openglwidget类是内嵌在主界面上的opengl界面的类，主要完成了显示和更新的实现
//类中重载了几个基本的函数：initializeGL()、paintGL()、resizeGL()
//initializeGL()函数主要用于对opengl界面进行初始的设置，
//resizeGL()函数主要用于对于窗口界面的设置，
//paintGL()函数是绘制函数，每次主界面刷新时都会自动调用此函数，所有要在界面上显示的操作都要在此函数中执行，
//在此函数中绘制坐标和各个角度值，使用global::area_display变量控制是否绘制计算区域的选择框
//使用global::startreview_clicked变量控制是否绘制计各个数据文件的回放内容
//根据回放数据文件的数据格式对应不同的绘制方式。
*/

#define MAX_CHAR  128
//unsigned __int16 buffer_opengl_start_show(800*3600); //在开始处理时，在此容器中存入一帧的数据用以显示
qint16 pb_buf[802*3600] = {0};
qint16 *pt_buf;


openglwidget::openglwidget(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::openglwidget)
{
    ui->setupUi(this);
    setMouseTracking(true);
}

void openglwidget::initializeGL()
{
    //QImage imag("C:/Users/FanJunyi/Desktop/color.PNG");
    //QBitmap test("C:/Users/FanJunyi/Desktop/color.PNG");
    //this->setMask(test);
    qDebug() << "opengL的高度" << this->height();
    qDebug() << "opengL的宽度" << float(float(this->height())/this->width());
    initializeOpenGLFunctions(); //必要语句
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glClearColor(0.925,0.925,0.925,0.0);
    //glClear(GL_COLOR_BUFFER_BIT);
    //glClearColor(19,48,80,1.0);

    /*glDisable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除颜色缓存和深度缓存*/
    //this->setAttribute(Qt::WA_TranslucentBackground,true);

    //坐标轴的显示列表的建立与设置
    list_coord_axis = glGenLists(1);
    glNewList(list_coord_axis,GL_COMPILE);
    glColor3f(0.0, 0.0, 0.0);//设置要绘制的颜色
    glPointSize(1.0); //设置当前绘制点的大小
    glBegin(GL_POINTS); //开始使用GL_POINTS模式进行绘制
    float R = 1; //绘制半径

    //开始绘制极坐标
    //绘制半径的圆
    int n =1201;
    for(int i=0;i<n;i++)
    {
        glVertex2f(R*cos(2*pi*i/n), R*sin(2*pi*i/n));
    }
    n=n-200;
    for(int i=0;i<n;i++)
    {
        glVertex2f((R-0.15)*cos(2*pi*i/n), (R-0.15)*sin(2*pi*i/n));
    }
    n=n-200;
    for(int i=0;i<n;i++)
    {
        glVertex2f((R-0.3)*cos(2*pi*i/n), (R-0.3)*sin(2*pi*i/n));
    }
    n=n-200;
    for(int i=0;i<n;i++)
    {
        glVertex2f((R-0.45)*cos(2*pi*i/n), (R-0.45)*sin(2*pi*i/n));
    }
    n=n-200;
    for(int i=0;i<n;i++)
    {
        glVertex2f((R-0.6)*cos(2*pi*i/n), (R-0.6)*sin(2*pi*i/n));
    }
    n=n-200;
    for(int i=0;i<n;i++)
    {
        glVertex2f((R-0.75)*cos(2*pi*i/n), (R-0.75)*sin(2*pi*i/n));
    }

    n=1000;
    //绘制角度线
    for(GLfloat i=-R;i<=R;i+=(2*R*2/n))
    {
        glVertex2f(i,0.0);
    }
    for(GLfloat i=-R;i<=R;i+=(2*R*2/n))
    {
        glVertex2f(i*(sqrt(3)/2),i*0.5);
    }
    for(GLfloat i=-R;i<=R;i+=(2*R*2/n))
    {
        glVertex2f(i*0.5,i*(sqrt(3)/2));
    }
    for(GLfloat i=-R;i<=R;i+=(2*R*2/n))
    {
        glVertex2f(0.0,i);
    }
    for(GLfloat i=-R;i<=R;i+=(2*R*2/n))
    {
        glVertex2f(-i*(sqrt(3)/2),i*0.5);
    }
    for(GLfloat i=-R;i<=R;i+=(2*R*2/n))
    {
        glVertex2f(i*0.5,-i*(sqrt(3)/2));
    }
    glEnd();
    glEndList();
    //坐标轴的显示列表设置结束

    //角度信息的显示初始化
    /*int firsttime_build_anglelists=1;
    if(firsttime_build_anglelists)
    {
        firsttime_build_anglelists=0;
        list_angle_build = glGenLists(MAX_CHAR);
        wglUseFontBitmaps(wglGetCurrentDC(),0,MAX_CHAR,list_angle_build);
        glListBase(list_angle_build);
    }
    //检测显示的数据
    file.setFileName("openglcheck_bb.txt");
    rrawBuf = new qint16[3600*rrawPerPoint];*/
}

void openglwidget::paintGL()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);

    //使用两个全局变量来控制数据开始回放
    if(global::startreview_clicked)
    {
        if(global::run_firsttime)
        {
            //将此时设置的颜色系统从ini文件中读出
            for(int m=0;m<16;m++)
            {
                QString b('a'+m); //实现小写字母从a到m的循环
                QPalette pal;
                pal = ini->value("/color/"+b).value<QPalette>();
                color[m][0] = pal.color(backgroundRole()).red();
                color[m][1] = pal.color(backgroundRole()).green();
                color[m][2] = pal.color(backgroundRole()).blue();
                qDebug() << "颜色" << color[m][0] << color[m][1] << color[m][2];
            } //颜色经检验无误
            maxStrength = ini->value("/color_strength/a").toInt(); //将设置的最大观测值输出为int类型
            miniStrength = ini->value("/color_strength/b").toInt(); //将设置的最小观测值输出为int类型
            data_format = ini->value("/dataformat/format").toString(); //从初始文件中读出选中的数据格式
            global::run_firsttime = false;
        }
        qDebug() << "data_format" << data_format;
        correctAngle = ini->value("/radar_information/openglAngle").toInt();
        if(data_format == "龙口数据格式")
        {
            longkou_display(); //调用龙口数据格式对应的显示函数
        }
        else if(data_format == "平潭数据格式")
        {
            qDebug() << "调用平潭绘制函数";
            pingtan_display(); //调用平潭数据格式对应的显示函数
        }
        else if(data_format == "台湾海峡数据格式")
        {
            taiwan_display(); //调用台湾海峡数据格式对应的显示函数
        }
        else if(data_format == "RRAW数据格式")
        {
            rraw_display(); //调用RRAW数据格式对应的显示函数
        }
    }

    //使用两个全局变量来控制开始处理时的数据显示
    if(global::start_play_online)
    {
        if(global::run_firsttime)
        {
            qDebug() << "开始显示";
            //将此时设置的颜色系统从ini文件中读出
            for(int m=0;m<16;m++)
            {
                QString b('a'+m); //实现小写字母从a到m的循环
                QPalette pal;
                pal = ini->value("/color/"+b).value<QPalette>();
                color[m][0] = pal.color(backgroundRole()).red();
                color[m][1] = pal.color(backgroundRole()).green();
                color[m][2] = pal.color(backgroundRole()).blue();
            } //颜色经检验无误
            maxStrength = ini->value("/color_strength/a").toInt(); //将设置的最大观测值输出为int类型
            miniStrength = ini->value("/color_strength/b").toInt(); //将设置的最小观测值输出为int类型
            correctAngle = ini->value("/radar_information/openglAngle").toInt();
            global::run_firsttime = false;
        }
        start_display();
    }
    glCallList(list_coord_axis); //调用显示坐标轴的显示列表
    //顺时针显示角度
    glRasterPos2f(-0.02f,0.92f);
    glCallLists(strlen(str1),GL_UNSIGNED_BYTE,str1);
    glRasterPos2f(0.45f,0.8f);
    glCallLists(strlen(str2),GL_UNSIGNED_BYTE,str2);
    glRasterPos2f(0.8f,0.45f);
    glCallLists(strlen(str3),GL_UNSIGNED_BYTE,str3);
    glRasterPos2f(0.92f,0.0f);
    glCallLists(strlen(str4),GL_UNSIGNED_BYTE,str4);
    glRasterPos2f(0.79f,-0.48f);
    glCallLists(strlen(str5),GL_UNSIGNED_BYTE,str5);
    glRasterPos2f(0.45f,-0.82f);
    glCallLists(strlen(str6),GL_UNSIGNED_BYTE,str6);
    glRasterPos2f(-0.02f,-0.94f);
    glCallLists(strlen(str7),GL_UNSIGNED_BYTE,str7);
    glRasterPos2f(-0.49f,-0.82f);
    glCallLists(strlen(str8),GL_UNSIGNED_BYTE,str8);
    glRasterPos2f(-0.85f,-0.48f);
    glCallLists(strlen(str9),GL_UNSIGNED_BYTE,str9);
    glRasterPos2f(-0.98f,0.0f);
    glCallLists(strlen(str10),GL_UNSIGNED_BYTE,str10);
    glRasterPos2f(-0.84f,0.46f);
    glCallLists(strlen(str11),GL_UNSIGNED_BYTE,str11);
    glRasterPos2f(-0.49f,0.8f);
    glCallLists(strlen(str12),GL_UNSIGNED_BYTE,str12);

    //计算区域选择框的显示
    if(global::area_display)
    {
        strength_max = ini->value("/radar_information/maximum").toInt();
        strength_min = ini->value("/radar_information/minimum").toInt();
        if(ini->value("/area/id").toInt() == 0) //若在计算区域选择界面中选择的是自定义方式
        {
            radio1 = ini->value("/area/radio1").toInt(); //最小半径
            radio2 = ini->value("/area/radio2").toInt(); //最大半径
            angle1 = ini->value("/area/angle1").toInt(); //起始角度
            angle2 = ini->value("/area/angle2").toInt(); //结束角度
            if(angle1 >= angle2)
            {
                zero_cross = true; //区域跨零
            }
            else
            {
                zero_cross = false; //区域不跨零
            }
        }
        else if(ini->value("/area/id").toInt() == 1) //若在计算区域选择界面中选择的是固定区域大小的方式
        {
            int point_angle = ini->value("/area/center_angle").toInt(); //中心点的角度
            int point_radio = ini->value("/area/center_radio").toInt(); //中心点的半径
            int angle_range = ini->value("/area/angle_range").toInt(); //计算区域的角度范围
            int radio_range = ini->value("/area/radio_range").toInt(); //计算区域的半径范围
            radio1 = point_radio - (radio_range/2); //最小的半径值
            radio2 = point_radio + (radio_range/2); //最大的半径值
            if(point_angle - (angle_range/2)<0) //判断最小角度是否小于0（跨零）
            {
                //算出起始和截止角度
                angle1 = 360 - abs(point_angle - (angle_range/2));
                angle2 = point_angle + angle_range/2;
                zero_cross = true; //区域跨零
            }
            else if(point_angle+angle_range/2>360) //判断最大角度是否大于360度（跨零）
            {
                angle1 = point_angle - angle_range/2;
                angle2 = point_angle + angle_range/2 - 360;
                zero_cross = true; //区域跨零
            }
            else
            {
                angle1 = point_angle - angle_range/2;
                angle2 = point_angle + angle_range/2;
                zero_cross = false; //区域不跨零
            }
        }
        region_choose_display(); //绘制计算区域选择框的函数
    }
}

void openglwidget::region_choose_display()
{
    glBegin(GL_POINTS);//开始使用GL_POINTS模式进行绘制
    GLfloat size = 10.0; //当前绘制的点的大小
    glPointSize(size); //设置当前绘制点的大小
    float R = 1;
    glColor3f(1.0,0.0,0.0);
    if(zero_cross == true)
    {
        n1_1_left = (360-angle1)/0.5; //算出弧线（小半径，0度左边）所需的点数
        n1_2_left = (360-angle1)/0.5; //算出弧线（大半径，0度左边）所需的点数
        n1_1_right = angle2/0.5; //算出弧线（小半径，0度右边）所需的点数
        n1_2_right = angle2/0.5; //算出弧线（大半径，0度右边）所需的点数
        n_2 = (radio2-radio1)/0.5; //算出两条直线所需的点数
        //根据弧长算出绘制所需的点数
        for(int i=0;i<n_2;i++) //绘制两条直线的点
        {
            glVertex2f((R*(radio1-strength_min+0.5*i)/(strength_max-strength_min))*sin(angle1*pi/180),(R*(radio1-strength_min+0.5*i)/(strength_max-strength_min))*cos(angle1*pi/180));
            glVertex2f((R*(radio1-strength_min+0.5*i)/(strength_max-strength_min))*sin(angle2*pi/180),(R*(radio1-strength_min+0.5*i)/(strength_max-strength_min))*cos(angle2*pi/180));
        }
        for(int i=0;i<n1_1_left;i++) //绘制弧线（小半径，0度左边）的点
        {
            glVertex2f((R*(radio1-strength_min)/(strength_max-strength_min))*sin((angle1+0.5*i)*pi/180),(R*(radio1-strength_min)/(strength_max-strength_min))*cos((angle1+0.5*i)*pi/180));
        }
        for(int i=0;i<n1_2_left;i++) //绘制弧线（大半径，0度左边）的点
        {
            glVertex2f((R*(radio2-strength_min)/(strength_max-strength_min))*sin((angle1+0.5*i)*pi/180),(R*(radio2-strength_min)/(strength_max-strength_min))*cos((angle1+0.5*i)*pi/180));
        }
        for(int i=0;i<n1_1_right;i++) //绘制弧线（小半径，0度右边）的点
        {
            glVertex2f((R*(radio1-strength_min)/(strength_max-strength_min))*sin(0.5*i*pi/180),(R*(radio1-strength_min)/(strength_max-strength_min))*cos(0.5*i*pi/180));
        }
        for(int i=0;i<n1_2_right;i++) //绘制弧线（大半径，0度右边）的点
        {
            glVertex2f((R*(radio2-strength_min)/(strength_max-strength_min))*sin(0.5*i*pi/180),(R*(radio2-strength_min)/(strength_max-strength_min))*cos(+0.5*i*pi/180));
        }
    }
    else if(zero_cross == false)
    {
        n1_1_right = (angle2-angle1)/0.5; //算出弧线（小半径，0度右边）所需的点数
        n1_2_right = (angle2-angle1)/0.5; //算出弧线（大半径，0度右边）所需的点数
        n_2 = (radio2-radio1)/0.5; //算出两条直线所需的点数
        //根据弧长算出绘制所需的点数
        for(int i=0;i<n_2;i++) //绘制两条直线的点
        {
            glVertex2f((R*(radio1-strength_min+0.5*i)/(strength_max-strength_min))*sin(angle1*pi/180),(R*(radio1-strength_min+0.5*i)/(strength_max-strength_min))*cos(angle1*pi/180));
            glVertex2f((R*(radio1-strength_min+0.5*i)/(strength_max-strength_min))*sin(angle2*pi/180),(R*(radio1-strength_min+0.5*i)/(strength_max-strength_min))*cos(angle2*pi/180));
        }
        for(int i=0;i<n1_1_right;i++) //绘制弧线（小半径，0度右边）的点
        {
            glVertex2f((R*(radio1-strength_min)/(strength_max-strength_min))*sin((angle1+0.5*i)*pi/180),(R*(radio1-strength_min)/(strength_max-strength_min))*cos((angle1+0.5*i)*pi/180));
        }
        for(int i=0;i<n1_2_right;i++) //绘制弧线（大半径，0度右边）的点
        {
            glVertex2f((R*(radio2-strength_min)/(strength_max-strength_min))*sin((angle1+0.5*i)*pi/180),(R*(radio2-strength_min)/(strength_max-strength_min))*cos((angle1+0.5*i)*pi/180));
        }
    }
    glEnd();
    qDebug() << "aaaaaa" ;
}

//RRAW数据格式下的回放方式
void openglwidget::rraw_display()
{
    //qDebug() << "调用绘制函数";
    float curSize = 0.5; //当前绘制的点的大小
    glPointSize(curSize); //设置当前绘制的点的大小
    rrawAngle = correctAngle; //绘制的修正角度
    if(rrawAngle < 0) //当修正角度小于0时
        rrawAngle = rrawAngle + 360;
    glBegin(GL_POINTS);
    for(int pos=0;pos<rrawPerPoint*3600;pos+=rrawPerPoint)
    {
        rrawAngle = rrawAngle + 0.1; //算出当前的角度值
        float space = (maxStrength-miniStrength)/12.0;
        for(int i=0;i<rrawPerPoint;i++)
        {
            rrawInten = rrawBuf[pos+i];
            //qDebug() << rrawInten;
            //rrawInten = 1.5*rrawInten;
            //rrawInten = rrawInten - noise;
            if(rrawInten < miniStrength)
            {
                rrawInten = miniStrength;
                //qDebug() << "0";
            }
            if(rrawInten > maxStrength)
            {
                rrawInten = maxStrength;
            }
            rrawInten = miniStrength + maxStrength - rrawInten;

            judge = floor(rrawInten/space);
            //qDebug() << rrawInten << judge;
            switch(judge){
            case 0:
                glColor3f(color[15][0],color[15][1],color[15][2]);
                break;
            case 1:
                glColor3f(color[14][0],color[14][1],color[14][2]);
                break;
            case 2:
                glColor3f(color[13][0],color[13][1],color[13][2]);
                break;
            case 3:
                glColor3f(color[12][0],color[12][1],color[12][2]);
                break;
            case 4:
                glColor3f(color[11][0],color[11][1],color[11][2]);
                break;
            case 5:
                glColor3f(color[10][0],color[10][1],color[10][2]);
                break;
            case 6:
                glColor3f(color[9][0],color[9][1],color[9][2]);
                break;
            case 7:
                glColor3f(color[8][0],color[8][1],color[8][2]);
                break;
            case 8:
                glColor3f(color[7][0],color[7][1],color[7][2]);
                break;
            case 9:
                glColor3f(color[6][0],color[6][1],color[6][2]);
                break;
            case 10:
                glColor3f(color[5][0],color[5][1],color[5][2]);
                break;
            case 11:
                glColor3f(color[4][0],color[4][1],color[4][2]);
                break;
            case 12:
                glColor3f(color[3][0],color[3][1],color[3][2]);
                break;
            case 13:
                glColor3f(color[2][0],color[2][1],color[0][2]);
                break;
            case 14:
                glColor3f(color[1][0],color[1][1],color[1][2]);
                break;
            case 15:
                glColor3f(color[0][0],color[0][1],color[0][2]);
                break;
            default:
                glColor3f(color[0][0],color[0][1],color[0][2]);
            }
            glVertex2f((1.0/rrawPerPoint)*(i+1)*sin(rrawAngle*pi/180),(1.0/rrawPerPoint)*(i+1)*cos(rrawAngle*pi/180));
        }
        /*for(int i=0;i<rrawPerPoint;i++)
        {
            glColor3f(1.0,0.0,0.0);
            glVertex2f(-(0.9/rrawPerPoint)*(i+1)*sin(rrawAngle*pi/180),-(0.9/rrawPerPoint)*(i+1)*cos(rrawAngle*pi/180));
        }*/
    }
    glEnd(); //完成绘制
}

//龙口数据格式下的回放方式
void openglwidget::longkou_display()
{
    //float curSize = 0.1; //当前绘制的点的大小
    glPointSize(0.1); //设置当前绘制的点的大小
    glBegin(GL_POINTS);
    for(int pos=0;pos<802*3600;pos+=802) //数据每802位一条线
    {
        count_ten_number = pb_buf[pos]; //读取此角度下是第几根线
        //线数从1到10计数，但小数部分需要从0.0到0.9,因此线数减一
        anglePoint = pb_buf[pos+1] + (count_ten_number-1)*0.1 + correctAngle; //读取出角度，加上线数作为小数部分
        if(anglePoint > 360)
            anglePoint = anglePoint - 360;
        else if(anglePoint < 0)
            anglePoint = anglePoint + 360;
        //emit change_current_orientation(anglePoint); //发送信号给主界面以实时改变“当前方位”的值
        for(int i=1;i<=800;i++)
        {
            intensity = pb_buf[pos+1+i];
            judge = floor(intensity/((maxStrength-miniStrength)/12.0));
            switch(judge){
            case 0:
                glColor3f(color[15][0],color[15][1],color[15][2]);
                break;
            case 1:
                glColor3f(color[14][0],color[14][1],color[14][2]);
                break;
            case 2:
                glColor3f(color[13][0],color[13][1],color[13][2]);
                break;
            case 3:
                glColor3f(color[12][0],color[12][1],color[12][2]);
                break;
            case 4:
                glColor3f(color[11][0],color[11][1],color[11][2]);
                break;
            case 5:
                glColor3f(color[10][0],color[10][1],color[10][2]);
                break;
            case 6:
                glColor3f(color[9][0],color[9][1],color[9][2]);
                break;
            case 7:
                glColor3f(color[8][0],color[8][1],color[8][2]);
                break;
            case 8:
                glColor3f(color[7][0],color[7][1],color[7][2]);
                break;
            case 9:
                glColor3f(color[6][0],color[6][1],color[6][2]);
                break;
            case 10:
                glColor3f(color[5][0],color[5][1],color[5][2]);
                break;
            case 11:
                glColor3f(color[4][0],color[4][1],color[4][2]);
                break;
            case 12:
                glColor3f(color[3][0],color[3][1],color[3][2]);
                break;
            case 13:
                glColor3f(color[2][0],color[2][1],color[0][2]);
                break;
            case 14:
                glColor3f(color[1][0],color[1][1],color[1][2]);
                break;
            case 15:
                glColor3f(color[0][0],color[0][1],color[0][2]);
                break;
            default:
                glColor3f(color[0][0],color[0][1],color[0][2]);
            }
            glVertex2f((1/800)*i*sin(anglePoint*pi/180),(1/800)*i*cos(anglePoint*pi/180));
        }
    }
    glEnd(); //完成绘制
    qDebug() << "完成了绘制";
    //if(global::playback_mutithread_show) //若绘制的变量为真
    //{
       // qDebug() << "改变当前帧数";
        //emit change_current_frame(); //发送信号，以改变主界面上的“当前帧数”的值
        //global::playback_mutithread_show = false; //关闭回放多线程中的显示
        //global::playback_mutithread_read = true; //开启回放多线程中的读取
    //}
    //update();
}

//平潭数据格式下的回放方式
void openglwidget::pingtan_display()
{
    qDebug() << "调用绘制函数";
    float curSize = 0.1; //当前绘制的点的大小
    glPointSize(curSize); //设置当前绘制的点的大小
    pingtan_angle = 0; //将角度归为0
    glBegin(GL_POINTS);
    for(int pos=0;pos<pingtan_sample_per_line*pingtan_line_one_frame;pos+=pingtan_sample_per_line)
    {
        pingtan_angle = pingtan_angle + 360.0/pingtan_line_one_frame; //算出当前的角度值
        for(int i=0;i<pingtan_sample_per_line;i++)
        {
            pingtan_intensity = pt_buf[pos+i];
            judge = floor(pingtan_intensity/((maxStrength-miniStrength)/12.0));
            switch(judge){
            case 0:
                glColor3f(color[15][0],color[15][1],color[15][2]);
                break;
            case 1:
                glColor3f(color[14][0],color[14][1],color[14][2]);
                break;
            case 2:
                glColor3f(color[13][0],color[13][1],color[13][2]);
                break;
            case 3:
                glColor3f(color[12][0],color[12][1],color[12][2]);
                break;
            case 4:
                glColor3f(color[11][0],color[11][1],color[11][2]);
                break;
            case 5:
                glColor3f(color[10][0],color[10][1],color[10][2]);
                break;
            case 6:
                glColor3f(color[9][0],color[9][1],color[9][2]);
                break;
            case 7:
                glColor3f(color[8][0],color[8][1],color[8][2]);
                break;
            case 8:
                glColor3f(color[7][0],color[7][1],color[7][2]);
                break;
            case 9:
                glColor3f(color[6][0],color[6][1],color[6][2]);
                break;
            case 10:
                glColor3f(color[5][0],color[5][1],color[5][2]);
                break;
            case 11:
                glColor3f(color[4][0],color[4][1],color[4][2]);
                break;
            case 12:
                glColor3f(color[3][0],color[3][1],color[3][2]);
                break;
            case 13:
                glColor3f(color[2][0],color[2][1],color[0][2]);
                break;
            case 14:
                glColor3f(color[1][0],color[1][1],color[1][2]);
                break;
            case 15:
                glColor3f(color[0][0],color[0][1],color[0][2]);
                break;
            default:
                glColor3f(color[0][0],color[0][1],color[0][2]);
            }
            glVertex2f(-(1/pingtan_sample_per_line)*(i+1)*sin(pingtan_angle*pi/180),-(1/pingtan_sample_per_line)*(i+1)*cos(pingtan_angle*pi/180));
        }
    }
    glEnd(); //完成绘制
    //emit change_current_frame(); //发送信号，以改变主界面上的“当前帧数”的值
}

//台湾海峡数据格式下的回放方式
void openglwidget::taiwan_display()
{
    float curSize = 0.1; //当前绘制的点的大小
    glPointSize(curSize); //设置当前绘制的点的大小
    int test;
    test = buffer_show.size()/482;
    glBegin(GL_POINTS);
    for(int pos=0;pos<482*test;pos+=482) //数据每802位一条线
    {
        count_ten_number = buffer_show.at(pos); //读取此角度下是第几根线
        //线数从1到10计数，但小数部分需要从0.0到0.9,因此线数减一
        anglePoint = buffer_show.at(pos+1) + (count_ten_number-1)*0.1; //读取出角度，加上线数作为小数部分
        for(int i=1;i<=480;i++)
        {
            intensity = buffer_show.at(pos+1+i);
            judge = floor(intensity/((maxStrength-miniStrength)/12.0));
            switch(judge){
            case 0:
                glColor3f(color[15][0],color[15][1],color[15][2]);
                break;
            case 1:
                glColor3f(color[14][0],color[14][1],color[14][2]);
                break;
            case 2:
                glColor3f(color[13][0],color[13][1],color[13][2]);
                break;
            case 3:
                glColor3f(color[12][0],color[12][1],color[12][2]);
                break;
            case 4:
                glColor3f(color[11][0],color[11][1],color[11][2]);
                break;
            case 5:
                glColor3f(color[10][0],color[10][1],color[10][2]);
                break;
            case 6:
                glColor3f(color[9][0],color[9][1],color[9][2]);
                break;
            case 7:
                glColor3f(color[8][0],color[8][1],color[8][2]);
                break;
            case 8:
                glColor3f(color[7][0],color[7][1],color[7][2]);
                break;
            case 9:
                glColor3f(color[6][0],color[6][1],color[6][2]);
                break;
            case 10:
                glColor3f(color[5][0],color[5][1],color[5][2]);
                break;
            case 11:
                glColor3f(color[4][0],color[4][1],color[4][2]);
                break;
            case 12:
                glColor3f(color[3][0],color[3][1],color[3][2]);
                break;
            case 13:
                glColor3f(color[2][0],color[2][1],color[0][2]);
                break;
            case 14:
                glColor3f(color[1][0],color[1][1],color[1][2]);
                break;
            case 15:
                glColor3f(color[0][0],color[0][1],color[0][2]);
                break;
            default:
                glColor3f(color[0][0],color[0][1],color[0][2]);
            }
            glVertex2f(-(1/480)*i*sin(anglePoint*pi/180),-(1/480)*i*cos(anglePoint*pi/180));
        }
    }
    glEnd(); //完成绘制

    qDebug() << "完成了绘制";
    emit change_current_frame(); //发送信号，以改变主界面上的“当前帧数”的值
    update();
}

//开始处理模式下的显示函数
void openglwidget::start_display()
{
    //QTime time;
    //time.start();
    float curSize = 1.5; //当前绘制的点的大小
    //start_angle = 0;
    glPointSize(curSize); //设置当前绘制的点的大小
    glBegin(GL_POINTS);
    for(int pos=0;pos<(point_num+1)*3600;pos+=point_num+1)
    {
        //start_angle = buffer_opengl_start_show[pos]/10.0;
        //start_angle = start_angle + 0.1;
        start_angle = buffer_opengl_start_show[pos]/10.0 + correctAngle;
        if(start_angle > 360)
            start_angle = start_angle - 360;
        else if(start_angle < 360)
            start_angle = start_angle + 360;
        //qDebug() << "start_angle" << start_angle;
        //countOnline++;
        //qDebug() << "opengllalala" << start_angle;
        for(int j=1;j<=point_num;j++)
        {
            start_strength = buffer_opengl_start_show[pos+j] - 8000;
            judge = floor(start_strength/((maxStrength - miniStrength)/12.0));
            switch(judge){
            case 0:
                glColor3f(color[15][0],color[15][1],color[15][2]);
                break;
            case 1:
                glColor3f(color[14][0],color[14][1],color[14][2]);
                break;
            case 2:
                glColor3f(color[13][0],color[13][1],color[13][2]);
                break;
            case 3:
                glColor3f(color[12][0],color[12][1],color[12][2]);
                break;
            case 4:
                glColor3f(color[11][0],color[11][1],color[11][2]);
                break;
            case 5:
                glColor3f(color[10][0],color[10][1],color[10][2]);
                break;
            case 6:
                glColor3f(color[9][0],color[9][1],color[9][2]);
                break;
            case 7:
                glColor3f(color[8][0],color[8][1],color[8][2]);
                break;
            case 8:
                glColor3f(color[7][0],color[7][1],color[7][2]);
                break;
            case 9:
                glColor3f(color[6][0],color[6][1],color[6][2]);
                break;
            case 10:
                glColor3f(color[5][0],color[5][1],color[5][2]);
                break;
            case 11:
                glColor3f(color[4][0],color[4][1],color[4][2]);
                break;
            case 12:
                glColor3f(color[3][0],color[3][1],color[3][2]);
                break;
            case 13:
                glColor3f(color[2][0],color[2][1],color[0][2]);
                break;
            case 14:
                glColor3f(color[1][0],color[1][1],color[1][2]);
                break;
            case 15:
                glColor3f(color[0][0],color[0][1],color[0][2]);
                break;
            default:
                glColor3f(color[0][0],color[0][1],color[0][2]);
            }
            glVertex2f((1.0/point_num)*j*sin(start_angle*pi/180),(1.0/point_num)*j*cos(start_angle*pi/180));
        }
        /*if(start_angle == 0)
        {
            for(int j=1;j<=point_num;j++)
            {
                glColor3f(color[0][0],color[0][1],color[0][2]);
                glVertex2f((0.9/point_num)*j*sin(start_angle*pi/180),(0.9/point_num)*j*cos(start_angle*pi/180));
            }
        }*/

    }
    glEnd(); //完成绘制
    //qDebug() << "opengl中的countOnline的值为" << countOnline;
    //qDebug() << "界面刷新";
    //qDebug() << "opengl主线程中绘制所需时间为" << time.elapsed();
    //update();
}

void openglwidget::receive_rraw_paint(int point,qint16 *buf)
{
    qDebug() << "回放" << point;
    rrawPerPoint = point;
    if(global::rrawFirst)
    {
        global::rrawFirst = false;
        cRRAW = 0;
        rrawBuf = new qint16[3600*rrawPerPoint];
    }
    cRRAW++;
    memcpy(&rrawBuf[((cRRAW-1)*rrawPerPoint*600)%(rrawPerPoint*3600)],buf,rrawPerPoint*600*sizeof(qint16));
    //if(cRRAW%6 == 0)
    qDebug() <<"回放接收数据";
    update();
    //paintGL();
}

//接收来自回放部分的龙口数据
void openglwidget::receive_pb_paint(qint16 *buf)
{
    c++;
    memcpy(&pb_buf[((c-1)*802*600)%(802*3600)],buf,802*600*sizeof(qint16));
    if(c%6 == 0)
        update();
}

//接收来自回放部分的平潭数据
void openglwidget::receive_pt_paint(int aa, int bb, int cc, int dd, qint16 *buf)
{
    pingtan_line_one_frame = aa; //一帧中的线数
    pingtan_sample_per_line = bb; //每条线的采样数
    pingtan_sample_start_distance = cc; //采样起始距离
    pingtan_sample_end_distance = dd; //采样结束距离
    if(pt_first)
    {
        pt_first = false;
        pt_buf = new qint16[pingtan_sample_per_line*3600];
    }
    pt++;
    qDebug() << "pt的值为" << pt;
    memcpy(&pt_buf[((pt-1)*pingtan_sample_per_line*600)%(pingtan_sample_per_line*3600)],buf,pingtan_sample_per_line*600*2);
    qDebug() << "2pt的值为" << pt;
    if(pt%6 == 0)
        update();
}

void openglwidget::receive_test_12252008(qint16 *bb,int num)
{
    qDebug() << "刷新opengl界面" << num;
    global::start_play_online = true; //打开opengl主线程中显示的变量
    global::run_firsttime = true;
    point_num = num; //每根线的字节数

    //int pos = (count-1)*(N*10*(1+num));
    //memcpy(&buffer_opengl_start_show[pos%(3600*(1+num))],bb,(1+num)*N*10*sizeof(qint16));
    qint16 angle = 0;
    for(int i=0;i<10*N;i++)
    {
        angle = bb[i*(1+num)];
        memcpy(&buffer_opengl_start_show[angle*(num+1)],&bb[i*(1+num)],(1+num)*sizeof (qint16));
    }
    /*for(int i=0;i<3600;i++)
    {
        qDebug() << "lllllll" << buffer_opengl_start_show[i*(num+1)];
    }*/

    qDebug() << "调用一次opengl函数and point_num" << bb[0] << point_num << num;
    update();
    //emit online_update();
    qDebug() << "opengl界面刷新函数调用成功";

    //检测显示的数据，三个地方，h中private最后的QFile file，initialize的最后一句为file设置名称
    /*
    if(file.open(QIODevice::Text|QIODevice::WriteOnly|QIODevice::Append))
    {
        qDebug() << "文件已存在";
        QTextStream texts(&file);
        for(int j=0;j<100;j++)
        {
            texts << bb[j*801];
            texts << endl;
        }
        //检查大数据中的值
        for(int j=0;j<100;j++)
        {
            texts << buffer_opengl_start_show[pos%(3600*801)+j*801] << "    ";
            for(int i=0;i<800;i++)
            {
                texts << buffer_opengl_start_show[pos%(3600*801)+j*801+i+1] << " ";
            }
            texts << endl;
        }
        file.close();
    }*/
}

//建立一个函数，用于回放时更新界面
//在更新完毕时，发送信号，以改变readdata中的一个值，相当于触发一个开关，使得在readdata那边可以开始swap数组

void openglwidget::area_choose()
{
    update();
}

void openglwidget::mousePressEvent(QMouseEvent *event)
{
    int width = this->frameGeometry().width(); //获取openglwidget窗口的宽度
    int height = this->frameGeometry().height(); //获取openglwidget窗口的高度

    int x = event->x(); //获取鼠标点击位置的x值
    int y = event->y(); //获取鼠标点击位置的y值
    //从ini文件中获取当前设置的计算区域
    QSettings *area = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int radio_s = area->value("/area/radio1").toInt();
    int radio_b = area->value("/area/radio2").toInt();
    int angle_s = area->value("/area/angle1").toInt();
    int angle_b = area->value("/area/angle2").toInt();
}

void openglwidget::resizeGL()
{
}


openglwidget::~openglwidget()
{
    delete []buffer_opengl_start_show;
    delete [] rrawBuf;
    delete ui;
}
