#include "figure.h"

//用于实时计算
figure::figure(double *&buf1,double *&buf2,QMutex *lock,QObject*parent):
    QObject(parent),bufx(buf1),bufz(buf2),lock_read(lock)
{

}

void figure::figure_thread1()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int min = ini->value("/radar_information/minimum").toInt();
    int max = ini->value("/radar_information/maximum").toInt();
    double sampleStep = ini->value("/radar_information/samplestep").toDouble();
    num_paint = (max - min) / sampleStep; //通过观测范围和采样间距计算出采样点数
    qDebug() << "figure_thread1中的num值为" << num_paint;
    qDebug() << "figure_thread1采样起始距离和采样结束距离和采样点数" << max << min << sampleStep;
    sample[1] = ini->value("/calculation_parameter/time_domain_points").toDouble(nullptr); //时域点数Nt
    sample[2] = ini->value("/calculation_parameter/airspace_points_x").toDouble(nullptr); //空域点数Nx
    sample[3] = ini->value("/calculation_parameter/airspace_points_y").toDouble(nullptr); //空域点数Ny
    sample[4] = ini->value("/calculation_parameter/sampling_interval_time").toDouble(nullptr); //时间抽样间隔derta(t)
    sample[5] = ini->value("/calculation_parameter/sampling_interval_x").toDouble(nullptr); //x抽样间隔derta(x)
    sample[6] = ini->value("/calculation_parameter/sampling_interval_y").toDouble(nullptr); //y抽样间隔derta(y)
    sample[7] = ini->value("/calculation_parameter/radar_correction_angle").toDouble(nullptr); //雷达修正角度
    sample[8] = ini->value("/calculation_parameter/water_depth").toDouble(nullptr); //水深
    sample[9] = 0; //观测界面的角度起始值
    qDebug() << "figure 运行到此处1";

    if(ini->value("/area/id").toInt() == 0) //若在计算区域选择界面中选择的是自定义方式
    {
        startradial = ini->value("/area/radio1").toInt(); //最小半径
        endradial = ini->value("/area/radio2").toInt(); //最大半径
        startangle = ini->value("/area/angle1").toInt(); //起始角度
        endangle = ini->value("/area/angle2").toInt(); //结束角度
    }
    else if(ini->value("/area/id").toInt() == 1) //若在计算区域选择界面中选择的是固定区域大小的方式
    {
        int point_angle = ini->value("/area/center_angle").toInt(); //中心点的角度
        int point_radio = ini->value("/area/center_radio").toInt(); //中心点的半径
        int angle_range = ini->value("/area/angle_range").toInt(); //计算区域的角度范围
        int radio_range = ini->value("/area/radio_range").toInt(); //计算区域的半径范围
        startradial = point_radio - (radio_range/2); //最小的半径值
        endradial = point_radio + (radio_range/2); //最大的半径值
        if(point_angle - (angle_range/2)<0) //判断最小角度是否小于0（跨零）
        {
            //算出起始和截止角度
            startangle = 360 - abs(point_angle - (angle_range/2));
            endangle = point_angle + angle_range/2;
        }
        else if(point_angle+angle_range/2>360) //判断最大角度是否大于360度（跨零）
        {
            startangle = point_angle - angle_range/2;
            endangle = point_angle + angle_range/2 - 360;
        }
        else
        {
            startangle = point_angle - angle_range/2;
            endangle = point_angle + angle_range/2;
        }
    }
    if(endangle > startangle)
    {
        nx = (endangle - startangle)*10; //nx = (angle_end - angle_start)*10;
    }
    else if(endangle <= startangle)
    {
        nx = (360 - startangle + endangle)*10; //nx = (360 - angle_start + angle_end)*10;
    }
    qDebug() << "figure 运行到此处2";
    start_count_radial = startradial/sampleStep; //计算区域在一个角度上，800个点中，起始的点数
    end_count_radial = endradial/sampleStep; //计算区域在一个角度上，800个点中，截止的点数
    ny = end_count_radial - start_count_radial; //在当前选定的计算区域内，存下一个角度上的点数
    qDebug() << "figure 运行到此处22";
    x = new double[nx+1]; //新建动态数组，用于存放当前计算区域下所有的极坐标角度
    z = new double[nx*ny+1]; //新建动态数组，用于存放当前计算区域下所有的强度值，数组大小和存储极坐标半径的数组大小一致
    y = new double[ny+1]; //实例化动态数组，存入极坐标半径
    for(int i=1;i<=ny;i++)
    {
        //点数×径向方向上点之间的距离，目前暂用derta(x)作为点之间的间隔
        y[i] = (start_count_radial+ i)*sample[5]; //将计算区域内的极坐标半径存入数组 \\sample_x
    }

    qDebug() << "figure 运行到此处3";
    dll.setFileName("radarDll"); //设置动态连接库的名称
    if(dll.load()) //绑定动态链接库，并判断是否成功绑定
    {
        qDebug() << "动态链接库载入成功";
        dll_isload = true; //绑定成功时，将0标志设为真
        interfun = (QTinterFun)dll.resolve("InterFun"); //对InterFun函数进行解析
        invers_calculation = (QTinversCalculate)dll.resolve("InversCalculate"); //对InversCalculate函数进行解析
    }
    else
    {
        qDebug() << "动态连接库载入失败";
        dll_isload = false; //绑定失败时，将标志设为假
    }
    qDebug() << "figure 运行到此处4";
    if(endangle > startangle)
    {
        nx = (endangle - startangle)*10; //nx = (angle_end - angle_start)*10;
    }
    else if(endangle <= startangle)
    {
        nx = (360 - startangle + endangle)*10; //nx = (360 - angle_start + angle_end)*10;
    }
    qDebug() << "figure 运行到此处5";
    //data_count = figure_calculate_data.length();  //获取数组的长度
    nt = 0; //将当前回放的帧数置零
    //选择岸基或者船载
    if(ini->value("/site_information/onBoard").toInt())
        sample[10] = 1;
    else if(ini->value("/site_information/shoreBased").toInt())
        sample[10] = 0;
    sample[11] = (endangle - startangle)/2 + startangle; //临时，当起始角度大于终止角度时不可用
    sample[12] = (start_count_radial+1)*7.5;
    sample[18] = 0;
    sample[19] = 0;
    sample[20] = 0;
    sample[21] = 0;
    sample[22] = 16.8; //船高
    qDebug() << "figure 运行到此处6";
    dataOut = new double[int(sample[2])*int(sample[3])*int(sample[1])+1]; //interfun函数的输出结果数组
    dataResult = new double[67*128*128+128*3+50]; //invers_calculation函数的输出结果数组
    qDebug() << "figure_thread1初始函数调用完毕";
    //QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    out = ini->value("/out_com/check").toInt(); //是否有惯导的输入
    if(out)
    {
        serial1 = new QSerialPort;
        serial1->setPortName(ini->value("/out_com/port").toString()); //设置端口号
        if(!serial1->open(QIODevice::ReadWrite))
            qDebug() << "惯导输入串口打开失败";
        serial1->setBaudRate(ini->value("/out_com/baud").toInt()); //设置波特率
        switch(ini->value("/out_com/bit").toInt() - 5) //数据位的选项为5 6 7 8，减5后分别变为0 1 2 3
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
        QString parity = ini->value("/out_com/parity").toString(); //设置校验位
        if(parity == "无")
            serial1->setParity(QSerialPort::NoParity);
        else if(parity == "奇校验")
            serial1->setParity(QSerialPort::OddParity);
        else if(parity == "偶校验")
            serial1->setParity(QSerialPort::EvenParity);
        QString stop = ini->value("/out_com/stop").toString(); //设置停止位
        if(stop == "1")
            serial1->setStopBits(QSerialPort::OneStop);
        else if(stop == "1.5")
            serial1->setStopBits(QSerialPort::OneAndHalfStop);
        else if(stop == "2")
            serial1->setStopBits(QSerialPort::TwoStop);
        serial1->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制
    }

    dataList << "时间" <<"表层流速" <<"表层流向"  <<"峰波频率" << "峰波周期" <<"峰波波向" << "有效波高" <<"信噪比"  <<"参数" <<"1/10波高" <<"1/10周期" <<"平均波高" <<"平均周期" <<"1/3周期" <<"动态角度" <<"峰波波长";
    set_time = ini->value("/database/settime").toString(); //读取数据库上一次存储时间（年月日）
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbTemp");
    //QSqlQuery query(db);//连接数据库
    if (!db.open())
    {
        QMessageBox::information(nullptr,"数据库连接失败","SQLite数据库连接失败，请检查软件版本并重新启动",1,QMessageBox::Ok);
    }
}

//每一帧后调用一次计算函数
void figure::figure_thread2(double *insData)
{
    //还需要从paintonline中发送一些需要的惯导数据过来
    qDebug() << "figure_thread2计算线程被调用" << dll_isload;
    nt++; //帧数自加
    memcpy(x,bufx,(nx+1)*sizeof (double)); //拷贝角度信息
    /*for(int kk = 0;;kk++)
        qDebug() << bufx[kk];*/
    memcpy(z,bufz,(nx*ny+1)*sizeof (double)); //拷贝雷达数据信息
    for(int i=0;i<5;i++) //拷贝惯导信息
    {
        sample[13+i] = insData[i];
    }
    if(dll_isload) //当动态链接库加载成功时
    {
        interfun(x,y,z,sample,nx,ny,nt,dataOut,startangle,endangle,2010); //调用第一个函数，输出为dataOut
        qDebug() << "figure_thread2一帧的计算函数成功被调用" << nx << ny << nt << startangle << endangle;
        //qDebug() << "begin" << "end" << begin << end;
        //QString out_data1 = QString::number(dataOut[1]);
        //QString out_data2 = QString::number(dataOut[7]);
    }
    //if(nt%int(sample[1]) == 0) //当帧数达到Nt时
    if(nt == 64)
    {
        if(invers_calculation)
        {
            invers_calculation(sample,dataOut,startangle,endangle,dataResult); //调用第二个函数，输出为dataResult
            //计算完毕后向playback发送信号，用于向主线程中发送信号，以改变主界面上的最终计算结果
            double fre_cor[128];
            double wave_spe[128];
            double wave_dir[128];
            double wavenum_fre[128*128];
            for(int i = 0; i<128; i++)
                fre_cor[i] = dataResult[51+i];
            for(int i = 0; i<128; i++)
            {
                wave_spe[i] = dataResult[51+128+i];
                //qDebug() << wave_spe[i];
            }

            for(int i = 0; i<128; i++)
                wave_dir[i] = dataResult[51+128*2+i];
            for(int i=0; i<128*128; i++)
                wavenum_fre[i] = dataResult[51+128*4+128*128*64];

            time = QDateTime::currentDateTime(); //获取当前系统的时间
            name = time.toString("ddhhmm"); //获取当前的年份

            emit result(name,dataResult[4],dataResult[5],dataResult[6],dataResult[7],fre_cor,wave_spe,wave_dir,wavenum_fre);
            qDebug() << "波高计算函数调用成功";

            qDebug() << "数据库打开与否" << db.open();


            //此时查找的时间和上次数据库的时间不一致，重新新建一个表单
            QSqlQuery query(QString("CREATE TABLE '%1' (id INTEGER PRIMARY KEY, v TEXT)").arg(name),db);
            if(!query.isActive())
                qWarning() << "MainWindow::DatabaseInit - ERROR: " << query.lastError().text();
            //create_sql = QString("create table student1 (id integer primary key, Firstname text,Lastname varchar,work varchar ,day varchar");
            qDebug() << "HHHH";
           if(!name.isEmpty())
           {
               for(int k=0;k<16;k++)
               {
                   /*add = QString("INSERT INTO '%1' ('%2') VALUES ('%3')").arg(name).arg(dataList[k]).arg(dataOut[k]);
                   query.prepare(add);
                   query.bindValue(":"+dataList[k], QString::number(dataOut[k+1]));
                   qDebug() << "lololo" << dataList[k];*/
                   if(!query.exec(QString("INSERT INTO '%1' (v) VALUES ('%2')").arg(name).arg(dataOut[k])))
                      emit databaseError();
                   //if(!query.exec(QString("INSERT INTO '%1' ('%2') VALUES ('%3')").arg(name).arg(dataList[k]).arg(dataOut[k])))
                       //emit databaseError();
               }
                  // QMessageBox::information(nullptr,"数据库存入错误","数据库存入计算结果值发生错误，无法正确存入数据",1,QMessageBox::Ok);
           }
           else
           {
               emit databaseError();
               //QMessageBox::information(nullptr,"数据库存入错误","数据库无法找到对应日期表，无法正确存入数据",1,QMessageBox::Ok);
           }
           if(out)
           {
               /*QByteArray byteout_data;
               byteout_data.resize(1);
               byteout_data.append(0x02,16);
               serial1->write(byteout_data);
               byteout_data.clear();
               byteout_data.append(0xFD,16);
               serial1->write(byteout_data);
               byteout_data.clear();
               byteout_data.append(0x12,16);
               serial1->write(byteout_data);
               QByteArray byteout_data1;
                byteout_data1.resize(1);
                byteout_data1.append(0x02,16);
               QByteArray byteout_data2;
               byteout_data2.resize(1);
               byteout_data2.append(0x02,16);
               QByteArray byteout_data3;
               byteout_data3.resize(1);
               byteout_data3.append(0x02,16);
               char check_sum = *byteout_data1.data() ^ *byteout_data2.data() ^ *byteout_data3.data();
               for(int k=0;k<16;k++)
               {
               }*/
           }
            /*
                double fre_cor[128];
                double wave_spe[128];
                double wave_dir[128];
                double wavenum_fre[128*128];
                for(int i = 0; i<128; i++)
                    fre_cor[i] = dataResult[51+i];
                for(int i = 0; i<128; i++)
                    {
                    wave_spe[i] = dataResult[51+128+i];
                    //qDebug() << wave_spe[i];
                }

                for(int i = 0; i<128; i++)
                    wave_dir[i] = dataResult[51+128*2+i];
                for(int i=0; i<128*128; i++)
                    wavenum_fre[i] = dataResult[51+128*4+128*128*64];
                ////计算完毕后向playback发送信号，用于向主线程中发送信号，以改变主界面上的最终计算结果
                emit chartvalue(dataResult[4],dataResult[5],dataResult[6],dataResult[7],fre_cor,wave_spe,wave_dir,wavenum_fre);
             */
        }
        nt = 0;
    }
}

figure::~figure()
{
    db.close();

}
