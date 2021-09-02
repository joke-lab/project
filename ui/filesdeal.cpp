#include "filesdeal.h"

filesdeal::filesdeal()
{
}

filesdeal::~filesdeal()
{
    delete [] excelValue;
    delete [] dataOut;
    delete [] dataResult;
}

void filesdeal::filesInitial_Slot()
{
    //每次点击开始回放按钮时，都要从初始文件中获取当前选定的计算区域的范围，得到起始角度、截止角度、起始半径和结束半径
    ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    if(ini->value("/area/id").toInt() == 0) //若在计算区域选择界面中选择的是自定义方式
    {
        radial_start = ini->value("/area/radio1").toInt(); //最小半径
        radial_end = ini->value("/area/radio2").toInt(); //最大半径
        angle_start = ini->value("/area/angle1").toInt(); //起始角度
        angle_end = ini->value("/area/angle2").toInt(); //结束角度
    }
    else if(ini->value("/area/id").toInt() == 1) //若在计算区域选择界面中选择的是固定区域大小的方式
    {
        int point_angle = ini->value("/area/center_angle").toInt(); //中心点的角度
        int point_radio = ini->value("/area/center_radio").toInt(); //中心点的半径
        int angle_range = ini->value("/area/angle_range").toInt(); //计算区域的角度范围
        int radio_range = ini->value("/area/radio_range").toInt(); //计算区域的半径范围
        radial_start = point_radio - (radio_range/2); //最小的半径值
        radial_end = point_radio + (radio_range/2); //最大的半径值
        if(point_angle - (angle_range/2)<0) //判断最小角度是否小于0（跨零）
        {
            //算出起始和截止角度
            angle_start = 360 - abs(point_angle - (angle_range/2));
            angle_end = point_angle + angle_range/2;
        }
        else if(point_angle+angle_range/2>360) //判断最大角度是否大于360度（跨零）
        {
            angle_start = point_angle - angle_range/2;
            angle_end = point_angle + angle_range/2 - 360;
        }
        else
        {
            angle_start = point_angle - angle_range/2;
            angle_end = point_angle + angle_range/2;
        }
    }
    startP = ((radial_start/ini->value("/radar_information/samplestep").toFloat())*10+5)/10; //计算区域在一个角度上起始的点数
    endP = radial_end/ini->value("/radar_information/samplestep").toFloat(); //计算区域在一个角度上截止的点数
    dll.setFileName("radarDll"); //设置动态连接库的名称
    if(dll.load()) //绑定动态链接库，并判断是否成功绑定
    {
        qDebug() << "动态链接库载入成功";
        dll_isload = true; //绑定成功时，将标志设为真
        interfun = (QTinterFun)dll.resolve("InterFun"); //对InterFun函数进行解析
        invers_calculation = (QTinversCalculate)dll.resolve("InversCalculate"); //对InversCalculate函数进行解析
    }
    else
    {
        qDebug() << "动态连接库载入失败";
        dll_isload = false; //绑定失败时，将标志设为假
    }
    //得到极坐标中角度的总个数
    if(angle_end > angle_start)
    {
        nx = (angle_end - angle_start)*10; //nx = (angle_end - angle_start)*10;
    }
    else if(angle_end <= angle_start)
    {
        nx = (360 - angle_start + angle_end)*10; //nx = (360 - angle_start + angle_end)*10;
    }
    ny = endP - startP; //在当前选定的计算区域内，存下的一个角度上的点数
    nt = 0; //将当前回放的帧数置零
    qDebug() << "nx" << nx << ny;
    x = new double[nx+1]; //新建动态数组，用于存放当前计算区域下所有的极坐标角度
    y = new double[ny+1]; //新建动态数组，用于存放当前计算区域下所有的极坐标半径
    z = new double[nx*ny+1]; //新建动态数组，用于存放当前计算区域下所有的强度值，数组大小和存储极坐标半径的数组大小一致
    sample[1] = ini->value("/calculation_parameter/time_domain_points").toDouble(nullptr); //时域点数Nt
    sample[2] = ini->value("/calculation_parameter/airspace_points_x").toDouble(nullptr); //空域点数Nx
    sample[3] = ini->value("/calculation_parameter/airspace_points_y").toDouble(nullptr); //空域点数Ny
    sample[4] = ini->value("/calculation_parameter/sampling_interval_time").toDouble(nullptr); //时间抽样间隔derta(t)
    sample[5] = ini->value("/calculation_parameter/sampling_interval_x").toDouble(nullptr); //x抽样间隔derta(x)
    sample[6] = ini->value("/calculation_parameter/sampling_interval_y").toDouble(nullptr); //y抽样间隔derta(y)
    sample[7] = ini->value("/calculation_parameter/radar_correction_angle").toDouble(nullptr); //雷达修正角度
    sample[8] = ini->value("/calculation_parameter/water_depth").toDouble(nullptr); //水深
    sample[9] = 0; //观测界面的角度起始值
    //sample[10] = 360; //观测界面的角度结束值
    dataOut = new double[int(sample[2])*int(sample[3])*int(sample[1])+1]; //interfun函数的输出结果数组
    dataResult = new double[67*128*128+128*3+50]; //invers_calculation函数的输出结果数组
    begin = angle_start; //读出所选区域的起始角度
    end = angle_end; //读出所选区域的结束角度
    //选择岸基或者船载
    if(ini->value("/site_information/onBoard").toInt())
        sample[10] = 1;
    else if(ini->value("/site_information/shoreBased").toInt())
        sample[10] = 0;
    if(angle_start < angle_end)
        sample[11] = (angle_end - angle_start)/2 + angle_start;
    else
    {
        if(360-angle_start < angle_end) //当360右边的区域更大
            sample[11] = angle_end - (360 - angle_start + angle_end)/2;
        else if(360-angle_start > angle_end) //当360度左边的区域更大
            sample[11] = angle_start + (360 - angle_start + angle_end)/2;
        else
            sample[11] = 0; //中心角度为0
    }
    qDebug() << "sample[11]" << sample[11];
    sample[12] = (startP+1)*sample[5];
    sample[13] = 0; //利用东速和北速计算出船速
    sample[14] = 0; //航向
    sample[15] = 0; //滚动
    sample[16] = 0; //俯仰
    sample[17] = 0; //升沉位移单位m
    sample[18] = 0;
    sample[19] = 0;
    sample[20] = 0;
    sample[21] = 0;
    sample[22] = 16.8; //船高
    //qDebug() << "sample数组值全部正确";
    //当需要输出txt文档时，从ini文件中获取路径
    check = ini->value("/batchprocess/txt").toInt();
    if(check)
    {
        txtPath = ini->value("/batchprocess/txtPath").toString();
    }
}

void filesdeal::rrawDeal_Slot(int line,QString f,QString t,QSqlQuery *query)
{
    qDebug() << "调用一次函数。。。。。。。。。。。。。。";
    //当需要输出txt文档时，从ini文件中获取路径
    check = ini->value("/batchprocess/txt").toInt();
    if(check)
    {
        txtPath = ini->value("/batchprocess/txtPath").toString();
    }

    //QSqlQuery query(db);//连接数据库
    /*if (!db.open())
    {
        QMessageBox::information(nullptr,"数据库连接失败","SQLite数据库连接失败，请检查软件版本并重新启动",1,QMessageBox::Ok);
    }*/
    file.setFileName(f);
    //此处应检测file的状态，是否已经打开，防止上次运行到一半强行关闭后重新运行时文件已经打开的状态
    file.open(QIODevice::ReadOnly);
    streamIn.setDevice(&file);
    frame = 0; //将当前帧数清零
    list.clear();
    qDebug() << "计算模式为RRAW数据格式";
    data = file.read(100).toHex(); //读取前100个数据头
    //采样帧数
    sample_frames_int = data.mid(78,8).left(2).toInt(&ok,16);
    qDebug() << "采样帧数" << sample_frames_int;
    //每条线的采样数
    sample_per_line_int = data.mid(86,8).left(2).toInt(&ok,16)+data.mid(86,8).left(4).right(2).toInt(&ok,16)*256;
    qDebug() << "每条线的采样数" << sample_per_line_int;
    //采样起始距离
    ssDis = data.mid(102,8).left(2).toInt(&ok,16)+data.mid(102,8).left(4).right(2).toInt(&ok,16)*256;
    qDebug() << "采样起始距离" << ssDis;
    //采样结束距离
    seDis = data.mid(110,8).left(2).toInt(&ok,16)+data.mid(110,8).left(4).right(2).toInt(&ok,16)*256;
    qDebug() << "采样结束距离" << seDis;
    decData = new qint16[sample_per_line_int]; //实例化存放一根线数据的数组
    rrawCountLine = 0; //当前帧的当前线数初始化
    //rrawforTen = 0; //一个角度的线数初始化
    rrawAngle = 0; //角度值初始化
    rrawFrame = 0; //帧数计数器设为0
    list.clear(); //用于计算的角度数组，初始化
    qDebug() << "各个角度值" << angle_start << angle_end << rrawAngle*10;
    //qDebug() << "..............." << t;
    while(!file.atEnd() && rrawFrame < 64)
    {
        data = file.read(1).toHex();
        if(data != "01")
        {
            data = file.read(128+2*sample_per_line_int);
            rrawAngle++;
            if(rrawAngle == 3200)
            {
                QByteArray arr = data.mid(48,80); //取出经度和纬度
                double insdata[10];
                memcpy(&insdata[0],arr.data(),8*10);
                sample[13] = sqrt(insdata[0]*insdata[0] + insdata[2]*insdata[2]); //由北速和东速算出船速
                sample[14] = insdata[7]; //航向
                sample[15] = insdata[6]; //滚动
                sample[16] = insdata[8]; //俯仰
                sample[17] = insdata[9]*0.001; //升沉位移单位m
            }
        }
        //qDebug() << data;
        else if(data == "01")
        {
            rrawCountLine++; //线计数器加一
            rrawAngle++; //角度值加一（角度值为10倍）
            data = file.read(128); //读取出惯导数据
            QByteArray arr = data.mid(48,80); //取出经度和纬度
            double insdata[10];
            memcpy(&insdata[0],arr.data(),8*10);
            sample[13] = sqrt(insdata[0]*insdata[0] + insdata[2]*insdata[2]); //由北速和东速算出船速
            sample[14] = insdata[7]; //航向
            sample[15] = insdata[6]; //滚动
            sample[16] = insdata[8]; //俯仰
            sample[17] = insdata[9]*0.001; //升沉位移单位m
            streamIn.readRawData((char *)decData,2*sample_per_line_int); //读取出一根线上的数据
            if(angle_start < angle_end) //计算区域不过零
            {
                //qDebug() << "data1";
                if(angle_start*10 <  rrawAngle &&  rrawAngle <= angle_end*10)
                {
                    x[rrawAngle - angle_start*10] = rrawAngle*0.1; //存入角度
                    //qDebug() << startP << endP << ny << "data2";
                    //list <<  rrawAngle*0.1;
                    //list <<  rrawAngle; //将double类型的角度存入与计算线程交换的内存中
                    //qDebug() << rrawAngle << "data2";
                    for(int j = startP+1;j<=endP;j++)
                    {
                        //list << decData[j];
                        z[(rrawAngle - angle_start*10 - 1)*ny+j-startP] = decData[j];
                    }
                    //qDebug() << "data3";
                }
            }
            else if(angle_start > angle_end) //计算区域跨零
            {
                if(rrawAngle <= angle_end*10)
                {
                    //qDebug() << "数据循环2";
                    int figurePos = int((3600 - angle_start*10) + rrawAngle);
                    x[figurePos] = rrawAngle*0.1;
                    for(int j = startP+1;j<=endP;j++)
                        z[(figurePos-1)*ny+j-startP] = decData[j];
                }
                else if(rrawAngle > angle_start*10)
                {
                    //qDebug() << "数据循环3";
                    x[rrawAngle - angle_start*10] = rrawAngle*0.1; //存入角度
                    for(int j = startP+1;j<=endP;j++)
                    {
                        z[(rrawAngle - angle_start*10 - 1)*ny+j-startP] = decData[j];
                    }
                }
            }
            //当角度为360度（数据满一帧后）
            //qDebug() << rrawAngle;
            if(rrawAngle == 3600)//if(rrawAngle == 3600 || file.atEnd())
            {
                //qDebug() << "yizhen";
                rrawAngle = 0; //数据角度格式化
                rrawFrame++; //当前帧数加一
                nt = rrawFrame; //当前是第几帧数据
                count_line = 0;
                for(int i=1;i<=ny;i++)
                {
                    //点数×径向方向上点之间的距离(delta(x)作为点之间的间隔)
                    y[i] = (startP + i)*sample[5]; //将计算区域内的极坐标半径存入数组
                }
                if(dll_isload) //当动态链接库加载成功时
                {
                    if(interfun)
                    {
                        //qDebug() << "计算";
                        interfun(x,y,z,sample,nx,ny,nt,dataOut,begin,end,2010); //调用第一个函数，输出为dataOut
                        //查看dataOut
                    }
                    else
                    {
                        qDebug() << "函数调用失败";
                    }
                }
                //qDebug() << nt;
                if(nt == int(sample[1])) //当帧数达到Nt时
                {
                    //qDebug() << "nt == int(sample[1])" << nt << sample[1];
                    if(invers_calculation)
                    {
                        //qDebug() << "波高计算函数调用成功";
                        invers_calculation(sample,dataOut,begin,end,dataResult); //调用第二个函数，输出为dataResult
                        //qDebug() << "有效波高" << dataResult[7];
                        QString d1 = QString::number(dataResult[6]); //有效波高
                        QString d2 = QString::number(dataResult[4]); //峰波周期
                        QString d3 = QString::number(dataResult[3]); //峰波频率
                        QString d4 = QString::number(dataResult[5]); //峰波波向
                        QString d5 = QString::number(dataResult[1]); //流速
                        QString d6 = QString::number(dataResult[2]); //流向
                        /*for(int m=0;m<8;m++)
                            qDebug() << dataResult[m+1];*/
                        QString yyyy = t.mid(0,4); //年
                        QString MM = t.mid(4,2); //月
                        QString dd = t.mid(6,2); //日
                        QString hh = t.mid(8,2); //时
                        QString mm = t.mid(10,2); //分
                        QString ss = t.mid(12,2); //秒
                        QString saveT = yyyy+"-"+MM+"-"+dd+" "+hh+":"+mm+":"+ss; //存储的时间格式
                        excelValue = new double[16];
                        memcpy(&excelValue[0],&dataResult[1],16*sizeof (double));
                        double fre_cor[128];
                        double wave_spe[128];
                        double wave_dir[128];
                        double wavenum_fre[128*128];
                        //for(int i = 0; i<128; i++)
                        //    fre_cor[i] = dataResult[51+i];
                        memcpy(&fre_cor[0],&dataResult[51],128*sizeof (double));
                        //for(int i = 0; i<128; i++)
                        //    wave_spe[i] = dataResult[51+128+i];
                        memcpy(&wave_spe[0],&dataResult[51+128],128*sizeof (double));
                        //for(int i = 0; i<128; i++)
                        //    wave_dir[i] = dataResult[51+128*2+i];
                        memcpy(&wave_dir[0],&dataResult[51+128*2],128*sizeof (double));
                        //for(int i=0; i<128*128; i++)
                        //    wavenum_fre[i] = dataRes ult[51+128*4+128*128*64];
                        memcpy(&wavenum_fre[0],&dataResult[51+128*4+128*128*64],128*128*sizeof (double));

                        delete [] excelValue;
                        for(int k=0;k<16;k++)
                        {
                            qDebug() << "ahhaha" << excelValue[k];
                        }
                        emit filesResult(line,insdata[4],insdata[5],excelValue,saveT);

                        if(check)
                        {
                            QDir data_save_dir;
                            if(!data_save_dir.exists(txtPath))
                            {
                                qDebug() << "运行设置中的存储路径不存在！";
                                txtPath = QCoreApplication::applicationDirPath();
                            }
                            data_save_dir.setCurrent(txtPath); //设置现在的目录为存储在ini文件中的目录
                            QFile file2(t + ".txt");
                            QDataStream streamIn2(&file2);
                            if(!file2.exists()) //判断此文件现在是否存在，不存在时新建
                            {
                                qDebug() << "文件已经存在";
                                file2.open(QIODevice::ReadWrite);
                                file2.close();
                            }
                            //streamIn2 << dataResult;
                            file2.open(QIODevice::WriteOnly|QIODevice::Append);
                            for(int i=0;;i++)
                                streamIn2 << QString::number(dataResult[i],'f',4) << " ";
                            //file2.write(QByteArray::fromRawData((char *)dataResult,(67*128*128+128*3+50)*sizeof (double)));
                            file2.close();
                        }

                        //存入数据库
                        //qDebug() << "数据库打开与否" << db.open();
                        QString tableName = yyyy;
                        qDebug() << "tableName" << tableName;
                        query->exec(QString("CREATE TABLE '%1' (Time TIME,fileN TEXT,Long TEXT,"
                                              "Lati TEXT,Hs TEXT,Tp TEXT,Fp TEXT,PeakD TEXT,Vr TEXT,"
                                              "Vt TEXT,SNR TEXT,Mo0 TEXT,H10 TEXT,T10 TEXT,Have TEXT,"
                                              "Tave TEXT,Ts TEXT,Aarea TEXT,Water TEXT)").arg(tableName));
                        if(tableName.toInt())
                        {
                            qDebug() << "tttttt" << t;
                            if(!t.isEmpty())
                            {
                                QString create = QString("INSERT INTO '%1' (Time,fileN,Long,Lati,Hs,Tp,Fp,"
                                                         "PeakD,Vr,Vt,SNR,Mo0,H10,T10,Have,Tave,Ts,Aarea,Water)"
                                                         "VALUES (:Time,:fileN,:Long,:Lati,:Hs,:Tp,:Fp,:PeakD,"
                                                         ":Vr,:Vt,:SNR,:Mo0,:H10,:T10,:Have,:Tave,:Ts,:Aarea,:Water)").arg(tableName);
                                int pos1 = f.lastIndexOf("/"); //在路径中寻找时间位置
                                QString fileN = f.mid(pos1); //文件名称
                                query->prepare(create);
                                query->bindValue(0,saveT); //存入时间
                                qDebug() << "!!!!" << " " << saveT;
                                query->bindValue(1,fileN); //存入文件名
                                //qDebug() << "!!!!" << " " << fileN;
                                query->bindValue(2,insdata[4]); //存入经度
                                //qDebug() << "!!!!" << " " << insdata[4];
                                query->bindValue(3,insdata[5]); //存入纬度
                                //qDebug() << "!!!!" << " " << insdata[5];
                                query->bindValue(4,dataResult[6]); //存入有效波高
                                //qDebug() << "!!!!" << " " << dataResult[6];
                                query->bindValue(5,dataResult[4]); //存入峰波周期
                                //qDebug() << "!!!!" << " " << dataResult[4];
                                query->bindValue(6,dataResult[3]); //存入峰波频率
                                //qDebug() << "!!!!" << " " << dataResult[3];
                                query->bindValue(7,dataResult[5]); //存入峰波波向
                                //qDebug() << "!!!!" << " " << dataResult[5];
                                query->bindValue(8,dataResult[1]); //存入流速
                                //qDebug() << "!!!!" << " " << dataResult[1];
                                query->bindValue(9,dataResult[2]); //存入流向
                                //qDebug() << "!!!!" << " " << dataResult[2];
                                for(int k=0;k<9;k++)
                                {
                                    query->bindValue(10+k,dataResult[7+k]);
                                    //qDebug() << "!!!!" << " " << dataResult[7+k];
                                }

                                if(!query->exec())
                                    QMessageBox::information(nullptr,"数据库存入错误","数据库存入计算结果值发生错误，无法正确存入数据",1,QMessageBox::Ok);

                                /*for(int k=0;k<16;k++)
                                {
                                    if(!query->exec(QString("INSERT INTO '%1' (v) VALUES ('%2')").arg(t).arg(dataResult[k+1])))
                                        qDebug() << "插入错误";
                                       //emit databaseError();
                                }
                                if(!query->exec(QString("INSERT INTO '%1' (v) VALUES ('%2')").arg(t).arg(t)))
                                    qDebug() << "插入错误";*/
                                   // QMessageBox::information(nullptr,"数据库存入错误","数据库存入计算结果值发生错误，无法正确存入数据",1,QMessageBox::Ok);
                            }
                            else
                            {
                                //emit databaseError();
                                QMessageBox::information(nullptr,"数据库存入错误","数据库无法找到对应日期表，无法正确存入数据",1,QMessageBox::Ok);
                            }
                        }
                    }
                }
            }
        }
    }
    file.close();
    //db.close();
}
