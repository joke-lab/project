#include "insdata.h"

insdata::insdata(double *array,QMutex *mut,QObject *parent):
    QObject(parent),buffer(array),mutex(mut)
{

}

void insdata::insData_grab_thread1()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    ins = ini->value("/ins_com/check").toInt(); //是否有惯导的输入
    if(ins)
    {
        serial1 = new QSerialPort;
        serial1->setPortName(ini->value("/ins_com/port").toString()); //设置端口号
        if(!serial1->open(QIODevice::ReadWrite))
            qDebug() << "惯导输入串口打开失败";
        serial1->setBaudRate(ini->value("/ins_com/baud").toInt()); //设置波特率
        switch(ini->value("/ins_com/bit").toInt() - 5) //数据位的选项为5 6 7 8，减5后分别变为0 1 2 3
        {
           case 0:
               serial1->setDataBits(QSerialPort::Data5);
                break;
           case 1:
               serial1->setDataBits(QSerialPort::Data6);
                break;
           case 2:
               serial1->setDataBits(QSerialPort::Data7);
                break;
           case 3:
               serial1->setDataBits(QSerialPort::Data8);
                break;
           default:
              break;
        }
        QString parity = ini->value("/ins_com/parity").toString(); //设置校验位
        if(parity == "无")
            serial1->setParity(QSerialPort::NoParity);
        else if(parity == "奇校验")
            serial1->setParity(QSerialPort::OddParity);
        else if(parity == "偶校验")
            serial1->setParity(QSerialPort::EvenParity);
        QString stop = ini->value("/ins_com/stop").toString(); //设置停止位
        if(stop == "1")
            serial1->setStopBits(QSerialPort::OneStop);
        else if(stop == "1.5")
            serial1->setStopBits(QSerialPort::OneAndHalfStop);
        else if(stop == "2")
            serial1->setStopBits(QSerialPort::TwoStop);
        serial1->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制
    }
    emit insinitialEnd(); //发送信号给主线程，开启其他信号
    qDebug() << "insdata中的函数insData_grab_thread被调用完毕";
}

void insdata::insData_grab_thread2()
{
    qDebug() << "insdata中的函数insData_grab_thread2被调用" << global::ins_start;
    /*serial1 = new QSerialPort;
    serial1->setPortName(ini->value("/gps_com/port").toString());
    if(!serial1->open(QIODevice::ReadWrite))
        qDebug() << "惯导输入串口打开失败";
    serial1->setParity(QSerialPort::NoParity);
    serial1->setBaudRate(115200);
    serial1->setPortName("com10");
    serial1->setDataBits(QSerialPort::Data8);
    serial1->setStopBits(QSerialPort::OneStop);*/
    qDebug() << "惯导输入串口的各个参数" << serial1->parity() << serial1->isOpen() << serial1->stopBits() << serial1->baudRate();
    //当有惯导数据输入且正在运行雷达时
    qDebug() << "insData中的判断变量" << ins << global::start_running_thread << global::ins_start;
    while(ins && global::start_running_thread)
    {
        QElapsedTimer t; //t的计时单位为毫秒
        t.start(); //一秒钟计时开始
        while(global::ins_start)
        {
            if(needmutexLock)
            {
                mutex->lock();
                ismutexLock = true;
            }
            needmutexLock = false; //将线程锁变量设为否，禁止重复进行上锁
            bool ok = true;
            qDebug() << "值" << ok << ins << global::ins_start;
            while(ok && ins && global::ins_start)
            {
                gpsData = serial1->readAll(); //从串口读入数据为字节
                gpsData_s = gpsData.toHex(); //从串口读入数据为字符串
                if(gpsData_s.contains("55aa"))
                {
                    gpsHead = gpsData_s.indexOf("55aa"); //找到第一个帧头的位置
                }
                while(gpsHead > -1) //寻找振头的位置
                {
                    gpsHeadAll << gpsHead;
                    gpsHead = gpsData_s.indexOf("55aa",gpsHead+1);
                }
                //QString strHex = QString::number( gpsData.at(gpsHead/2), 16);//帧头转16进制
                qDebug() << "insData线程中的判断变量" << gpsHeadAll.length() << ((gpsData_s.length() - gpsHeadAll[0])<159);
                if(gpsHeadAll.length() == 1 & (gpsData_s.length() - gpsHeadAll[0])<159)
                {
                    //重新抓捕一个惯导信号
                    //外加一个判断循环，在这里使用Break打破内层循环？？
                    break; //return;
                    //下面成功读取数据后改变变量
                }
                else
                {
                    //qDebug() << "insData中运行到此处";
                    for(int i = 0;i<gpsHeadAll.length();i++)
                    {
                        if(i < gpsHeadAll.length()-1)
                        {
                            gpsLength = gpsHeadAll[i+1] - gpsHeadAll[i];
                        }
                        else
                        {
                            //获取最后一个数据头后面的数据的长度，用于判断此数据包是否完整
                            gpsLength = gpsData_s.length() - gpsHeadAll[i];
                        }

                        if(gpsLength == 160)
                        {
                            //根据QString的位置找到在QByte的位置并获取其后的一段长度的字节
                            //此段字节包含惯导数据中所需的所有有效信息
                            gpsHead = gpsHeadAll[i]/2;
                            gpsDataPack = gpsData.mid(gpsHead,80);
                            //输出gpsDataPack的前两位是不是55aa////////////////////////////////////////

                            //gpsDataBUnInt用于存储“导航时间”和“用于分包发送附加结构体”
                            //第4个字节开始，共8个字节是无符号整型；
                            //qDebug() << "insData中的count值2";
                            QByteArray floatData = gpsDataPack.mid(11,60); //取出数据段
                            float floatArray[16];
                            memcpy(&floatArray[0],floatData.data(),4*10); //前10个double型数据一次性存入
                            QByteArray tempData = floatData.mid(41,8); //取出经度纬度
                            memcpy(&floatArray[10],tempData.data(),2*4);
                            tempData.clear();
                            tempData = floatData.mid(49,12); //取出滚动航向俯仰
                            memcpy(&floatArray[12],tempData.data(),3*4);
                            tempData.clear();
                            tempData = gpsDataPack.mid(77,2);; //取出升沉位移
                            double doubleArray[16];
                            for(int i=0;i<15;i++)
                            {
                                doubleArray[i] = floatArray[i];
                            }
                            memcpy(&doubleArray[15],tempData.data(),2); //升沉位移
                            count++;
                            //qDebug() << "insData中的count值为" << count;
                            for(int i=0;i<16;i++)
                            {
                                qDebug() << floatArray[i];
                            }
                            if(count < 101)
                            {
                                buffer[0] = count;
                                //qDebug() << "insData中获取的惯导数据的组数为" << count;
                                memcpy(&buffer[count*64+1],&doubleArray,16*sizeof (double));
                                //qDebug() << "----" << doubleArray[10] << doubleArray[11] << doubleArray[12] << doubleArray[13] << doubleArray[15];
                            }
                           // ok = false; //改变外层循环判据，跳出循环，表明已经读取到正确惯导数据
                            i = gpsHeadAll.length(); //已经读到完整的数据包，跳出for循环
                            qDebug() << "insData中buffer的第一个值为" << buffer[0];
                        }
                     }
                 }
            }
            ok = false;
        }
        if(ismutexLock)
        {
            mutex->unlock();
            ismutexLock = false;
            qDebug() << "ins解锁";
        }
        //while(t.elapsed() < 39)
            //QCoreApplication::processEvents();
        needmutexLock = true;
        count = 0; //将计数清零
    }
    serial1->close(); //当退出循环时，关闭串口，便于下一次重新打开
}

insdata::~insdata()
{

}
