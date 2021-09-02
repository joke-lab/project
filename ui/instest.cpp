#include "instest.h"
#include "ui_instest.h"
#include <QDebug>

instest::instest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::instest)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    QObject::connect(ui->test,SIGNAL(clicked(bool)),this,SLOT(test_Slot()));
    QObject::connect(ui->stop,SIGNAL(clicked(bool)),this,SLOT(stop_Slot()),Qt::DirectConnection);
    QObject::connect(ui->exit,SIGNAL(clicked(bool)),this,SLOT(exit_Slot()));

}

//开始测试按钮的槽函数
void instest::test_Slot()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int open = ini->value("/gps_com/check").toInt(); //判断当前惯导串口是否打开
    if(!open)
    {
        QMessageBox::warning(nullptr,"串口未打开","惯导输入的串口未打开，请先在串口设置中打开惯导串口",QMessageBox::Ok,0);
        return;
    }
    QSerialPort serial;
    serial.setPortName(ini->value("/gps_com/port").toString()); //设置端口号

    serial.setBaudRate(ini->value("/gps_com/baud").toInt()); //设置波特率
    switch(ini->value("/gps_com/bit").toInt() - 5) //数据位的选项为5 6 7 8，减5后分别变为0 1 2 3
    {
       case 0:
           serial.setDataBits(QSerialPort::Data5);
            break;
       case 1:
           serial.setDataBits(QSerialPort::Data6);
            break;
       case 2:
           serial.setDataBits(QSerialPort::Data7);
            break;
       case 3:
           serial.setDataBits(QSerialPort::Data8);
            break;
       default:
          break;
    }
    QString parity = ini->value("/gps_com/bit").toString(); //设置校验位
    if(parity == "无")
        serial.setParity(QSerialPort::NoParity);
    else if(parity == "奇校验")
        serial.setParity(QSerialPort::OddParity);
    else if(parity == "偶校验")
        serial.setParity(QSerialPort::EvenParity);
    QString stop = ini->value("/gps_com/stop").toString(); //设置停止位
    if(stop == "1")
        serial.setStopBits(QSerialPort::OneStop);
    else if(stop == "1.5")
        serial.setStopBits(QSerialPort::OneAndHalfStop);
    else if(stop == "2")
        serial.setStopBits(QSerialPort::TwoStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制
    if(!serial.open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(nullptr,"串口打开失败","惯导输入串口打开失败，请检查设置。",QMessageBox::Ok,0);
    }

    while(!isstop)
    {
        qDebug() << "串口测试开始读取数据";
        gpsData = serial.readAll(); //从串口读入数据为字节
        gpsData_s = gpsData.toHex(); //从串口读入数据为字符串
        if(gpsData_s.contains("55aa"))
        {
            gpsHead = gpsData_s.indexOf("55aa"); //找到第一个帧头的位置
        }
        if(!gpsHeadAll.empty())
            gpsHeadAll.clear();
        while(gpsHead > -1) //寻找振头的位置
        {
            gpsHeadAll << gpsHead;
            gpsHead = gpsHead = gpsData_s.indexOf("55aa",gpsHead+1);
        }
        QString strHex = QString::number( gpsData.at(gpsHead/2), 16);//帧头转16进制
        if(gpsHeadAll.length() == 1 && (gpsData_s.length() - gpsHeadAll[0]<159))
        {
            //重新抓捕一个惯导信号
            //外加一个判断循环，在这里使用Break打破内层循环？？
            //break;
            //下面成功读取数据后改变变量
        }
        else
        {
            int i = gpsHeadAll.length()-1; //直接定位到最后一个数据头
            gpsLength = gpsData_s.length() - gpsHeadAll[i]; //获取最后一个数据头后面的数据的长度，用于判断此数据包是否完整
            //当最后一个数据包不完整的时候，定位到前一个数据包，前一个数据包一定是完整的
            if(gpsLength != 160) //主要限制一个数据包不完整的情况80个字节160个字符
                i = i-1;

            gpsHead = gpsHeadAll[i]/2;
            //根据QString的位置找到在QByte的位置并获取其后的一段长度的字节
            //此段字节包含惯导数据中所需的所有有效信息
            gpsDataPack = gpsData.mid(gpsHead,80);
            //gpsDataBUnInt用于存储“导航时间”和“用于分包发送附加结构体”
            //第4个字节开始，共8个字节是无符号整型；
            gpsDataBUnInt = gpsDataPack.mid(3,8);
            memcpy(&gpsDataUnInt,gpsDataBUnInt.data(),4*2); //QByteArray转int的一种方法
            //40个字节是float
            gpsDataBFloat = gpsDataPack.mid(11,40);
            memcpy(&gpsDataFloat,gpsDataBFloat,4*10);
            //用变量gpsDataBUnInt来存储经纬度信息
            gpsDataBUnInt.clear();
            gpsDataBUnInt = gpsData.mid(51,8);
            memcpy(&gpsDataInt,gpsDataBUnInt.data(),4*2);
            //用刚才存储40个字节float的变量来存储滚动、航向和俯仰
            gpsDataBFloat.clear();
            gpsDataBFloat = gpsDataPack.mid(59,12);
            memcpy(&gpsDataFloatRYP,gpsDataBFloat,4*3);
            //用刚才存储经纬度的变量来存储UTC时间
            gpsDataBUnInt.clear();
            gpsDataBUnInt = gpsDataPack.mid(71,4);
            memcpy(&gpsDataUnIntTime,gpsDataBUnInt.data(),4); //unsigned int类型UTC时间的变量
            gpsDataBUnInt.clear();
            gpsDataBUnInt = gpsDataPack.mid(75,2);
            memcpy(&gpsDataUnShortTime,gpsDataBUnInt.data(),2); //unsigned short类型UTC时间的变量
            gpsDataFloatTime = gpsDataUnIntTime/1000.0*65536 + gpsDataUnShortTime/1000.0; //计算出UTC时间
            gpsDataIntTime = floor(gpsDataFloatTime*1000.0);
            //用刚才存储UTC时间的变量来存储升沉位移
            gpsDataBUnInt.clear();
            gpsDataBUnInt = gpsDataPack.mid(77,2);
            memcpy(&gpsDataHeave,gpsDataBUnInt.data(),2);
            ui->longitude_edit->setText(QString::number(gpsDataInt[0]));
            ui->latitude_edit->setText(QString::number(gpsDataInt[1]));
            ui->scroll_edit->setText(QString::number(gpsDataFloatRYP[0]));
            ui->course_edit->setText(QString::number(gpsDataFloatRYP[1]));
            ui->pitch_edit->setText(QString::number(gpsDataFloatRYP[2]));
            ui->utc_edit->setText(QString::number(gpsDataIntTime));
            ui->heave_edit->setText(QString::number(gpsDataHeave/1000)); //升沉单位m
            ui->shipspeed_edit->setText(QString::number(sqrt(pow(gpsDataFloat[8],2)+pow(gpsDataFloat[6],2)))); //利用东速和北速计算出船速
        }
    }
}

//停止测试按钮的槽函数
void instest::stop_Slot()
{
    isstop = false;
}

//退出按钮的槽函数
void instest::exit_Slot()
{
    this->close();
}

instest::~instest()
{
    delete ui;
}
