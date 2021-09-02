#include "waveview.h"
#include "ui_waveview.h"

waveview::waveview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::waveview)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    setAttribute(Qt::WA_StyledBackground, true);
    //this->setWindowFlag(Qt::FramelessWindowHint);//无边框显示
    //按钮组件，将三个radiobutton加入并设定对应的序列号
    radioButtonGroup = new QButtonGroup;
    radioButtonGroup->addButton(ui->radio1,0);
    radioButtonGroup->addButton(ui->radio2,1);
    //从初始文件中读出各个参数默认值，并显示在各个下拉框内
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    bool id = ini->value("/Waveform_display/id").toBool();
    if(id)
    {
        ui->radio1->setChecked(true);
        ui->radio2->setChecked(true);
        ui->label->setText(ini->value("/Waveform_display/angle").toString());
        ui->label_2->setEnabled(true);
    }
    else
    {
        ui->radio1->setChecked(true);
        ui->radio2->setChecked(true);
        ui->label_2->setEnabled(false);
        ui->label->setText(ini->value("/Waveform_display/angle").toString());
        ui->label->setEnabled(false);
        ui->label_2->setEnabled(false);
    }
    p = new QCustomPlot;
    p = ui->wavedispaly;
    //设置坐标轴的名称范围
    p->xAxis->setLabel("采样点数");
    p->yAxis->setLabel("回波强度");
    framenum = (ini->value("radar_information/maximum").toInt()-ini->value("radar_information/minimum").toInt())/ini->value("radar_information/samplestep").toFloat();
    p->xAxis->setRange(0,framenum*2);
    p->yAxis->setRange(0,13000);
    //添加曲线（构造函数设置，方便后面的曲线重构）
    p->addGraph(); //添加一条曲线
    //ui->wavedispaly->graph(0)->setPen(QPen(Qt::black)); //设置线条颜色
    //设置坐标轴缩放
    p->setInteraction(QCP::iRangeZoom);

    QObject::connect(ui->start,SIGNAL(clicked(bool)),this,SLOT(openSlot())); //开始-信号与槽的链接
    QObject::connect(ui->stop,SIGNAL(clicked(bool)),this,SLOT(stopSlot())); //停止-信号与槽的链接
    QObject::connect(ui->quit,SIGNAL(clicked(bool)),this,SLOT(quitSlot())); //退出-信号与槽的链接
    QObject::connect(ui->radio1,SIGNAL(clicked(bool)),this,SLOT(radiobuttonSlot()));  //同下为角度选择方式的radiobutton的信号与槽连接
    QObject::connect(ui->radio2,SIGNAL(clicked(bool)),this,SLOT(radiobuttonSlot()));
}

waveview::~waveview()
{
    delete ui;
}

//根据按钮组的Id号选择不同的模式
void waveview::radiobuttonSlot()
{

    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    switch(radioButtonGroup->checkedId())
    {
    case 0:
        ui->radio1->setEnabled(true);
        ui->radio2->setEnabled(true);
        ui->label->setEnabled(false);
        ui->label_2->setEnabled(false);
        ini->setValue("/Waveform_display/id","0");
        ini->setValue("/Waveform_display/angle",ui->label->text());
        break;
    case 1:
        ui->radio1->setEnabled(true);
        ui->radio2->setEnabled(true);
        ui->label->setEnabled(true);
        ui->label_2->setEnabled(true);
        ini->setValue("/Waveform_display/id","1");
        ini->setValue("/Waveform_display/angle",ui->label->text());
        break;
    default:
        break;
    }
}

//添加数据画图
void waveview::addDataandShow()
{
    p->removeGraph(0);
    p->addGraph();
    p->graph(0)->setPen(QPen(Qt::black)); //设置线条颜色

    //判断是回放还是实时
    //可变数组存放绘图的坐标的数据，分别存放x和y坐标的数据，()内为数据长度
    QVector<double> x(framenum), y(framenum);
    for(int i = 1; i <= framenum; ++i)
    {
        x[i-1] = i;
    }
    for(int i = 0; i < num;++i)
    {
        memcpy(&y[0],&w_buf[i*framenum],framenum * sizeof (qint16));
    }

    p->graph(0)->setData(x,y);
    p->replot(QCustomPlot::rpQueuedReplot);

    //删除图形的另一种方式(现在使用删除图层方法)
    //p->graph(0)->data().data()->clear();
    /*
    //数据以点的形式增加
    p->graph(0)->addData(mCount-1, mData);
    mCount++;//当前点个数
    mData++;//新增点的值
    if(mCount >= framenum)//显示横坐标>120是移动X坐标系
    {
        p->xAxis->setRange(mCount-framenum, mCount);
    }
    p->replot();
    */
}


//区分实时/回放函数
 int choose(int a_num,int r_c, qint16 *w_buf)
{
    int angle;
    int N = 10;
    //qint16 p_c; //判断当前角度(实时)
    if(a_num == 10*N)
    {
        angle = w_buf[0];
    }
    else
    {
        ++r_c;
        angle = r_c * 6 * N;
        if(r_c == 6)
            r_c = 0;
    }
    return angle;
}

//按钮判断
int choose_2(int angle, bool id)
{
    int fix_angle; //固定角度
    int correct_angle; //修正角度
    int show_angle; //显示角度
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    correct_angle = ini->value("/radar_information/openglAngle").toFloat() * 10;
    if(id)
    {
        fix_angle = ini->value("/Waveform_display/angle").toFloat() * 10;
        show_angle = (fix_angle - correct_angle + 3600) % 3600; //取余数只能整型
    }
    else
    {
        show_angle = (angle * 10 - correct_angle + 3600) % 3600;
    }
    return  show_angle;
}

//鼠标移动显示函数
void waveview::myMoveEvent(QMouseEvent *event)
{
    //鼠标坐标
    int x_pos = event->pos().x();
    int y_pos = event->pos().y();

    //鼠标坐标转换为内部坐标
    float x_val = p->xAxis->pixelToCoord(x_pos);
    float y_val = p->yAxis->pixelToCoord(y_pos);

    QString str,strToolTip;
    str = QString::number(x_val,10,3);
    strToolTip += "x:";
    strToolTip += str;
    strToolTip += "\n";

    for (int i = 0;i < p->xAxis->graphs().count();i++) {
            //获得x轴坐标位置对应的曲线上y的值
            float y = p->graph(i)->data()->at(x_val)->value;
            str = QString::number(y);
            strToolTip += "y" + QString::number(i) + "：";
            strToolTip += str;
            strToolTip += "\n";
        }
        QToolTip::showText(cursor().pos(), strToolTip, p);
}

//保存图片函数
void waveview::save_p()
{
    p->saveJpg("wave",300,400);
}

//开始按钮的槽函数
void waveview::openSlot(int x_num, int angle_num, qint16 *wave_buf)
{
    //
    if(x_num == 0)
        return;
    global::waveshow = true;

    if(x_num != framenum)
    {
        n_angle = wave_buf[0];
        for(int i = 0; i < 10*N; ++i)
        {

            memcpy(&w_buf[i*framenum],&w_buf[i*(framenum+1)],framenum*sizeof (qint16));
        }
    }
    else
    {
        ++r_c;
        n_angle = r_c * 6 * N;
        if(r_c == 6)
            r_c = 0;
       memcpy(&w_buf[0],&wave_buf[0],x_num * angle_num * sizeof (qint16));
    }
    //
    num = angle_num;
    n_angle = choose_2(n_angle,id);
    mTimer = new QTimer(this);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(addDataandShow()));
    mTimer->start(4);

}

//停止按钮的槽函数
void waveview::stopSlot()
{
    global::waveshow = false;

}

//退出按钮的槽函数
void waveview::quitSlot()
{
    global::waveshow = false;
    this->close();
}
