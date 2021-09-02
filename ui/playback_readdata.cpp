#include "playback_readdata.h"
/*
//playback_readdata类是一个继承自QObject的子线程类，主要用于在回放数据的时候运行数据读取的耗时函数，与openglwidget主线程并行运行
//此类中只有一个函数，函数中是一个大的while循环，便于主线程对其进行停止和开始的操作
//在循环中针对回放的不同数据格式，分为了三个部分：龙口数据格式、平潭数据格式和台湾海峡数据格式
//此类中建立了两个全局的向量容器buffer_read和buffer_show，读取的数据存储在buffer_read中，读取的数据达到360度时，
//和buffer_show做交换，最新读取的数据就存放到了buffer_show中供opengl显示，在龙口数据格式和台湾海峡数据格式中，
//交换后就将buffer_read清空为零，重新对其进行数据插入，而平潭数据格式中将数据直接赋值到相应的位置，而不是对其进行清空
*/

QVector<unsigned __int16>buffer_read; //对全局变量进行初始化
QVector<unsigned __int16>buffer_show; //对全局变量进行初始化


QList<double>figure_read_data;
QList<double>figure_calculate_data;
int radial_start_para = 0; //计算区域在一个角度上，800个点中，起始的点数
int radial_end_para = 0; //计算区域在一个角度上，800个点中，截止的点数
int angle_start = 0; //计算区域的起始角度
int angle_end = 0; //计算区域的结束角度
int count_frame = 0; //用于计数，当前是第几帧数据

playback_readdata::playback_readdata(QObject*parent):
    QObject(parent)
{

}

playback_readdata::~playback_readdata()
{
}

/*
//反异步干扰函数
void playback_readdata::CoCIRFun(int rrawAngleCount)
{
    if(rrawAngleCount < 3) //线数必须大于3
        return;
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    //bool state = ini->value("/radar_information/state").toBool(); //判断是否需要反异步干扰的状态
    int fGateStep = ini->value("/radar_information/gatestep").toInt();
    int nGrade = ini->value("/radar_information/grade").toInt();
    float fGate = fGateStep * nGrade;
    tempData = new qint16[sample_per_line_int]; //存放反异步干扰数据的数组
    tempData1 = new qint16[sample_per_line_int];
    tempData2 = new qint16[sample_per_line_int];
    //qDebug()<<"反异步干扰"<<fGate;
    for(int i = 0; i < rrawAngleCount; i++)
    {
        int nPre = i - 1;
        int nData = i;
        int nAft = i + 1;
        if(nPre < 0)
        {
            nPre = rrawAngleCount-1;
        }

        if(nAft >= rrawAngleCount)
        {
            nAft = 0;
        }

        memcpy(&tempData1[0],&RRAW_buffer[nPre*sample_per_line_int],sample_per_line_int*sizeof(qint16)); //第一根线的数据复制
        memcpy(&tempData[0],&RRAW_buffer[nData*sample_per_line_int],sample_per_line_int*sizeof(qint16)); //第二根线的数据复制,(基准线)
        memcpy(&tempData2[0],&RRAW_buffer[nAft*sample_per_line_int],sample_per_line_int*sizeof(qint16)); //第三根线的数据复制

        for(int j = 0; j < sample_per_line_int; j++)
        {
            float fGateTemp1 = fabs(tempData1[j] - tempData[j]);
            float fGateTemp2 = fabs(tempData2[j] - tempData[j]);

            if(fGateTemp1 > fGate && fGateTemp2 > fGate)
                tempData[j] = (tempData1[j] + tempData2[j])/2;
        }
        memcpy(&RRAW_buffer[i*sample_per_line_int],&tempData[0],sample_per_line_int*sizeof(qint16));
        //memcpy(&rraw_buffer[i*sample_per_line_int],&tempData[0],sample_per_line_int*sizeof(qint16));
    }
}
*/

//读取数据的耗时函数
void playback_readdata::startread_thread()
{
        if(first_run)
        {
            first_run = false;
            emit started_readdata_message(); //向playback中发回返回信息
        }

        //每次点击开始回放按钮时，都要从初始文件中获取当前选定的数据格式
        QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        data_format = ini->value("/dataformat/format").toString(); //从ini文件中读取出数据回放的格式
        filename_text = ini->value("review_file/filename").toString(); //从ini文件中读取文件名称
        state = ini->value("/radar_information/state").toBool(); //状态变量设置

        //读出反异步干扰的状态、门限
        //bool state = ini->value("/radar_information/state").toBool(); //判断是否需要反异步干扰的状态
        //int fGateStep = ini->value("/radar_information/gatestep").toInt();
        //int nGrade = ini->value("/radar_information/grade").toInt();
        //float fGate = fGateStep * nGrade; //抑制门限的判断

        //每次点击开始回放按钮时，都要从初始文件中获取当前选定的计算区域的范围，得到起始角度、截止角度、起始半径和结束半径
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
        radial_start_para = radial_start/ini->value("radar_information/samplestep").toFloat(); //计算区域在一个角度上，800个点中，起始的点数
        radial_end_para = radial_end/ini->value("radar_information/samplestep").toFloat(); //计算区域在一个角度上，800个点中，截止的点数

        emit playback_figure_message(); //发送回playback，以表明已经成功打开回放计算子线程
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

        ny = radial_end_para - radial_start_para; //在当前选定的计算区域内，存下的一个角度上的点数
        nt = 0; //将当前回放的帧数置零

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
        if(ini->value("/site_information/onBoard").toInt())
            sample[10] = 1;
        else if(ini->value("/site_information/shoreBased").toInt())
            sample[10] = 0;
        sample[11] = (angle_end - angle_start)/2 + angle_start;
        sample[12] = (radial_start_para+1)*7.5;
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
        //sample[10] = 360; //观测界面的角度结束值
        dataOut = new double[int(sample[2])*int(sample[3])*int(sample[1])+1]; //interfun函数的输出结果数组
        dataResult = new double[67*128*128+128*3+50]; //invers_calculation函数的输出结果数组
        begin = angle_start; //读出所选区域的起始角度
        end = angle_end; //读出所选区域的结束角度

        if(data_format == "RRAW数据格式")
        {
            int pos2 = filename_text.indexOf("2");
            QString search = filename_text.mid(pos2,14);
            //发送至主界面显示的数据时间
            QString dataTime = search.mid(0,4) + "-" + search.mid(4,2) + "-" + search.mid(6,2) + "-" + search.mid(8,2) + "-" + search.mid(10,2) + "-" + search.mid(12,2);
            file1.setFileName(filename_text); //设置文件的名称
            file1.open(QIODevice::ReadWrite); //打开文件，设置为读写模式
            streamIn.setDevice(&file1); //设置数据流读入的设备file1
            rrawFrame = 0; //帧数计数器设为0
            data = file1.read(100).toHex(); //读取前100个数据头
            //采样帧数
            sample_frames_int = data.mid(78,8).left(2).toInt(&ok,16);
            qDebug() << "采样帧数" << sample_frames_int;
            //每条线的采样数
            sample_per_line_int = data.mid(86,8).left(2).toInt(&ok,16)+data.mid(86,8).left(4).right(2).toInt(&ok,16)*256;
            qDebug() << "每条线的采样数" << sample_per_line_int;
            //采样起始距离
            sample_start_distance_int = data.mid(102,8).left(2).toInt(&ok,16)+data.mid(102,8).left(4).right(2).toInt(&ok,16)*256;
            qDebug() << "采样起始距离" << sample_start_distance_int;
            //采样结束距离
            sample_end_distance_int = data.mid(110,8).left(2).toInt(&ok,16)+data.mid(110,8).left(4).right(2).toInt(&ok,16)*256;
            qDebug() << "采样结束距离" << sample_end_distance_int;

            cal_buffer = new qint16[nx*sample_per_line_int];
            decData = new qint16[sample_per_line_int]; //实例化存放一根线数据的数组
            //QVector<qint16>decDatatemp; //实例化存放一根线数据的数组
            rraw_buffer = new qint16[600*sample_per_line_int]; //实例化存放数据的数组（用于显示）
            RRAW_buffer = new qint16[3600*sample_per_line_int]; //实例化存储一帧数据数组（用于计算）
            //QVector<QVector<qint16>>RRAW_buffer; //实例化存储一帧数据二维数组（用于计算）

            rrawCountLine = 0; //当前帧的当前线数初始化(用于显示)
            //rrawforTen = 0; //一个角度的线数初始化
            int correctAngle = ini->value("/calculation_parameter/radar_correction_angle").toInt()*10;
            rrawInitialAngle = 0 + correctAngle; //角度值初始化
            if(rrawInitialAngle > 3600)
                rrawInitialAngle = rrawInitialAngle - 3600;
            else if(rrawInitialAngle < 0)
                rrawInitialAngle = rrawInitialAngle + 3600;
            rrawAngle = rrawInitialAngle; //将进行角度修正后的初始角度赋值给角度
            rrawAngleCount = 0; //角度计数器置零
            figure_read_data.clear(); //用于计算的角度数组，初始化
            QFile file2;
            QString fi = "C:/Users/FanJunyi/Desktop/x.txt";
            QTextStream stream2;
            file2.setFileName(fi);
            if(!file2.exists()) //判断此文件现在是否存在，不存在时新建
            {
                qDebug() << "文件已经存在";
                file2.open(QIODevice::ReadWrite);
                file2.close();
            }
            stream2.setDevice(&file2);

            global::rrawFirst = true;
            while(!file1.atEnd() && rrawFrame < 64)
            {
                data = file1.read(1).toHex();
                //当此时的角度为359.9时，角度置零

                if(data != "01")
                {
                    data = file1.read(128+2*sample_per_line_int);
                    rrawAngle++;
                    if(rrawAngle == 3600)
                        rrawAngle = 0;
                    rrawAngleCount++;
                }
                else if(data == "01")
                {
                    rrawCountLine++; //线计数器加一
                    rrawAngle++; //角度值加一（角度值为10倍）
                    if(rrawAngle == 3600)
                        rrawAngle = 0.0;
                    rrawAngleCount++;
                    data = file1.read(128); //读取出惯导数据
                    QByteArray arr = data.mid(48,80); //取出经度和纬度
                    double insdata[10];
                    memcpy(&insdata[0],arr.data(),8*10);
                    sample[13] = sqrt(insdata[0]*insdata[0] + insdata[2]*insdata[2]); //由北速和东速算出船速
                    sample[14] = insdata[7]; //航向
                    sample[15] = insdata[6]; //滚动
                    sample[16] = insdata[8]; //俯仰
                    sample[17] = insdata[9]*0.001; //升沉位移单位m
                    streamIn.readRawData((char *)decData,2*sample_per_line_int); //读取出一根线上的数据

                    //复制线数据进入rraw_buffer数组
                    //memcpy(&rraw_buffer[(rrawCountLine-1)*sample_per_line_int],&decData[0],sample_per_line_int*sizeof(qint16)); //非反异步干扰用memcpy效率更高
                    for(int i=0;i<sample_per_line_int;i++)
                    {
                        rraw_buffer[i+(rrawCountLine-1)*sample_per_line_int] = decData[i];
                    }

                    //复制线数据进入RRAW_buffer数组
                    memcpy(&RRAW_buffer[(rrawAngleCount-1)*sample_per_line_int],&decData[0],sample_per_line_int*sizeof (qint16));
                    //memcpy(&RRAW_buffer.at(rrawAngleCount-1),&decData[0],sample_per_line_int*sizeof (qint16));

                    /*
                    //X,Z赋值（一根线做选取）
                    if(angle_start < angle_end)
                    {
                        if(angle_start*10<rrawAngle && rrawAngle<=angle_end*10)
                        {
                            x[rrawAngle - angle_start*10] = rrawAngle*0.1; //存入角度
                            //qDebug() << startP << endP << ny << "data2";
                            //list <<  rrawAngle*0.1;
                            //list <<  rrawAngle; //将double类型的角度存入与计算线程交换的内存中
                            //qDebug() << rrawAngle << "data2";
                            for(int j = radial_start_para+1;j<=radial_end_para;j++)
                            {
                                //list << decData[j];
                                z[(rrawAngle - angle_start*10 - 1)*ny+j-radial_start_para] = decData[j];
                            }
                        }
                    }
                    else if(angle_start > angle_end)
                    {
                        if(rrawAngle <= angle_end*10)
                        {
                            //qDebug() << "数据循环2";
                            int figurePos = int((3600 - angle_start*10) + rrawAngle);
                            x[figurePos] = rrawAngle*0.1;
                            for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                z[(figurePos-1)*ny+j-radial_start_para] = decData[j];
                        }
                        else if(rrawAngle > angle_start*10)
                        {
                            //qDebug() << "数据循环3";
                            x[rrawAngle - angle_start*10] = rrawAngle*0.1; //存入角度
                            for(int j = radial_start_para+1;j<=radial_end_para;j++)
                            {
                                z[(rrawAngle - angle_start*10 - 1)*ny+j-radial_start_para] = decData[j];
                            }
                        }
                    }

                    */

                    //X赋值（线数据做选取）
                    if(angle_start < angle_end)
                    {
                        if(angle_start*10<rrawAngle && rrawAngle<=angle_end*10)
                        {
                            x[rrawAngle - angle_start*10] = rrawAngle*0.1; //存入角度
                            //qDebug() << startP << endP << ny << "data2";
                            //list <<  rrawAngle*0.1;
                            //list <<  rrawAngle; //将double类型的角度存入与计算线程交换的内存中
                            //qDebug() << rrawAngle << "data2";
                        }
                    }
                    else if(angle_start > angle_end)
                    {
                        if(rrawAngle <= angle_end*10)
                        {
                            //qDebug() << "数据循环2";
                            int figurePos = int((3600 - angle_start*10) + rrawAngle);
                            x[figurePos] = rrawAngle*0.1;
                        }
                        else if(rrawAngle > angle_start*10)
                        {
                            //qDebug() << "数据循环3";
                            x[rrawAngle - angle_start*10] = rrawAngle*0.1; //存入角度
                        }
                    }

                    //当角度为360度（数据满一帧后）（用于计算）
                    if(rrawAngleCount == 3600)
                    {
                        qDebug() << "数据满一帧后";
                        rrawAngleCount = 0; //数据角度格式化
                        rrawFrame++; //当前帧数加一
                        nt = rrawFrame; //当前是第几帧数据
                        count_line = 0;

                        if(state)
                        {
                            commonfunction common;
                            common.CoCIRFun(3600,sample_per_line_int,RRAW_buffer);
                        }
                        qDebug() << "数据满一帧后1";
                        /*QVector<QVector<double>>aa;
                        QVector<double>bb;
                        aa << bb;
                        QVector<QVector<qint16>>cal_buffer;*/

                        //Z赋值（一帧数据做选取）
                        if(angle_start < angle_end)
                        {
                            //cal_buffer = new qint16[(angle_end-angle_start)*10*sample_per_line_int];
                            memcpy(&cal_buffer[0],&RRAW_buffer[angle_start*10*sample_per_line_int],nx*sample_per_line_int*sizeof(qint16)); //计算区域数组
                            for(int k = 0; k < nx; k++)
                            {
                                qDebug() << "数据满一帧后1k" << k;
                                for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                    {
                                    //z[(rrawAngle - angle_start*10 - 1)*ny+j-radial_start_para] = cal_buffer[k*sample_per_line_int+j];
                                    z[k*(radial_end_para-radial_start_para)+j-(radial_start_para+1)+1] = cal_buffer[k*sample_per_line_int+j];
                                    qDebug() << "数据满一帧后1j" << j;
                                }
                            }
                            qDebug() << "数据满一帧后12";
                        }
                        else if(angle_start > angle_end)
                        {
                            //cal_buffer = new qint16[(angle_end-angle_start+360)*10*sample_per_line_int];
                            memcpy(&cal_buffer[(360-angle_start)*10*sample_per_line_int],&RRAW_buffer[0],angle_end*10*sample_per_line_int*sizeof (qint16));
                            memcpy(&cal_buffer[0],&RRAW_buffer[angle_start*10*sample_per_line_int],(360-angle_start)*10*sample_per_line_int*sizeof (qint16));

                            for(int k = 0; k < nx; k++)
                            {
                                for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                    z[0] = cal_buffer[k*sample_per_line_int+j];
                            }
                            /*if(rrawAngle <= angle_end*10)
                            {
                                //qDebug() << "数据循环2";
                                int figurePos = int((3600 - angle_start*10) + rrawAngle);
                                //x[figurePos] = rrawAngle*0.1;
                                for(int k = 0; k < nx; k++)
                                {
                                    for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                        z[(figurePos-1)*ny+j-radial_start_para] = cal_buffer[k*sample_per_line_int+j];
                                }

                            }
                            else if(rrawAngle > angle_start*10)
                            {
                                //qDebug() << "数据循环3";
                                //x[rrawAngle - angle_start*10] = rrawAngle*0.1; //存入角度
                                for(int k = 0; k < nx; k++)
                                {
                                    for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                    {
                                        z[(rrawAngle - angle_start*10 - 1)*ny+j-radial_start_para] = cal_buffer[k*sample_per_line_int+j];
                                    }
                                }
                            }*/
                        }

                        qDebug() << "数据满一帧后2";
                        for(int i=1;i<=ny;i++)
                        {
                            //点数×径向方向上点之间的距离(delta(x)作为点之间的间隔)
                            y[i] = (radial_start_para + i)*sample[5]; //将计算区域内的极坐标半径存入数组
                        }
                        qDebug() << "数据满一帧后3";
                        if(dll_isload) //当动态链接库加载成功时
                        {
                            qDebug() << "数据满一帧后4";
                            if(interfun)
                            {
                                qDebug() << "函数调用成功";

                                /*if(file2.open(QIODevice::WriteOnly|QIODevice::Append))
                                {
                                    stream2 << "nt" << nt;
                                    stream2 << "\r";
                                    for(int i=0;i<nx+1;i++)
                                    {
                                        stream2 << QString::number(x[i]) << " ";
                                    }
                                    stream2 << "\r";
                                }*/
                                qDebug() << "over" << nx << ny << nt << begin << end;

                                file2.close();
                                interfun(x,y,z,sample,nx,ny,nt,dataOut,begin,end,2010); //调用第一个函数，输出为dataOut
                                //查看dataOut
                            }
                            else
                            {
                                qDebug() << "函数调用失败";
                            }
                        }
                        if(nt == int(sample[1])) //当帧数达到Nt时
                        {
                            if(invers_calculation)
                            {
                                qDebug() << "波高计算函数调用成功";

                                invers_calculation(sample,dataOut,begin,end,dataResult); //调用第二个函数，输出为dataResult
                                qDebug() << "计算的输入参数";

                                ////计算完毕后向playback发送信号，用于向主线程中发送信号，以改变主界面上的最终计算结果
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
                                //    wavenum_fre[i] = dataResult[51+128*4+128*128*64];
                                memcpy(&wavenum_fre[0],&dataResult[51+128*4+128*128*64],128*128*sizeof (double));
                                ////计算完毕后向playback发送信号，用于向主线程中发送信号，以改变主界面上的最终计算结果
                                emit playback_figure_returnvalue(dataTime,dataResult[1],dataResult[1],dataResult[2],dataResult[4],dataResult[5],dataResult[6],dataResult[16],fre_cor,wave_spe,wave_dir,wavenum_fre);
                            }
                        }
                    }
                    //每10个角度往主界面发送惯导数据
                    if(rrawAngleCount%100 == 0)
                    {
                        emit rrawInsShow(insdata[4],insdata[5],sample[13],insdata[7]);
                        //qDebug() << "发闪送" << insdata[4] << insdata[5] << sample[13] << insdata[7];
                    }

                    //当线数计数器满600时，角度共60度，调用opengl绘制一次界面（用于显示）
                    if(rrawCountLine == 600)
                    {
                        if(state)//根据设定，确定是否进行反异步干扰处理
                        {
                            commonfunction common;
                            common.CoCIRFun(600,sample_per_line_int,rraw_buffer);
                        }
                        //qDebug() << "绘制一次";
                        rrawCountLine = 0; //将记录当前是第几根线的变量置为0
                        if(rrawFirstSwap)
                        {
                            rrawFirstSwap = false;
                            global::startreview_clicked = true; //开启主线程的画图
                            global::run_firsttime = true;
                            //emit start_showing(); //发送信号，通知playback中开启绘制线程中的变量
                        }
                        qint16 tem[600*sample_per_line_int];
                        memcpy(tem,rraw_buffer,600*sample_per_line_int*sizeof (qint16));
                        //将数据发送给opengl
                        emit rraw_paint(sample_per_line_int,tem);
                        //发送至波形显示
                        if(global::waveshow)
                            emit st_ws(sample_per_line_int,600,tem);
                    }
                }
            }
        }
        if(data_format == "龙口数据格式") //当选定的数据格式为“龙口数据格式”时
        {
            emit longkou_sampling_step(); //龙口数据格式：发送给主界面用于更新采样步长
            longkou_firsttime_swap = true;
            count_frame = 0; //用于记录当前是第几帧数据的变量，初始值设为0
            figure_read_data.clear();
            int pos2 = filename_text.indexOf("1");
            QString search = filename_text.mid(pos2,12);
            //发送至主界面显示的数据时间
            QString dataTime = search.mid(0,2) + "-" + search.mid(2,2) + "-" + search.mid(4,2) + "-" + search.mid(6,2) + "-" + search.mid(8,2) + "-" + search.mid(10,2);
            file1.setFileName(filename_text); //设置文件的名称
            file1.setFileName(filename_text); //设置文件的名称
            file1.open(QIODevice::ReadWrite); //打开文件，设置为只读模式
            streamIn.setDevice(&file1); //设置数据流读入的设备file1
            lk_c = 1; //将线数清零，在再次点击开始回放时确保正确
            angle_count = 0; //将累计的角度数清零，在再次点击开始回放时确保正确
            count_to_ten = 0; //将一个角度下的数据线数清零，在再次点击开始回放时确保正确
            int correctAngle = ini->value("/calculation_parameter/radar_correction_angle").toInt();
            qDebug() << "correctAngle" << correctAngle;
            //同时具有playback类中设为真和opengl类中设为真
            //当文件未读完时
            while(!file1.atEnd() && count_frame < 64) //当绘制部分在读取数据时，在此while中循环
            {
                data = file1.read(16).toHex(); //读取第一行的数据
                if(data.startsWith("a5a53412a5a53412")) //当读取到数据头时
                {
                    angle = data.right(12).left(4); //取出坐标
                    decAngle = angle.left(2).toInt(&ok,16) + angle.right(2).toInt(&ok,16)*256; //将坐标转换为十进制
                    decAngle = decAngle + correctAngle; //加入计算修正角度
                    if(decAngle > 360)
                        decAngle = decAngle%360;
                    else if(decAngle < 0)
                        decAngle = decAngle + 360;
                    data = file1.read(16).toHex(); //第二行数据无效
                    streamIn.readRawData((char*)&decRadial,1600); //将一根线上的所有强度值一次性读入
                    if(lk_fl) //当此根数据线是文件的第一根线时
                    {
                        lk_fl = false;
                        decAngle_last = decAngle; //此时的坐标为第一根数据线的坐标
                        angle_count++; //在此帧输入的角度个数上加一
                        count_to_ten++; //角度计数加一，代表了此数据线是当前角度的第几根线
                        lk_buffer[0] = count_to_ten; //将此数据线是当前角度的第几根线的数值存入
                        lk_buffer[1] = decAngle; //向全局容器中写入角度值
                        //根据角度值判断是否属于计算区域内的值
                        if(angle_start < angle_end)
                        {
                            if(angle_start <= decAngle && decAngle <= angle_end)
                            {
                                figure_read_data << decAngle + 0.1*(count_to_ten-1); //将double类型的角度存入与计算线程交换的内存中
                                for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                {
                                    figure_read_data << decRadial[j];
                                }
                            }
                        }
                        else if(angle_start > angle_end)
                        {
                            if(decAngle <= angle_end || decAngle >= angle_start)
                            {
                                figure_read_data << decAngle + 0.1*(count_to_ten-1); //将double类型的角度存入与计算线程交换的内存中
                                for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                {
                                    figure_read_data << decRadial[j];
                                }
                            }
                        }
                        memcpy(&lk_buffer[2],decRadial,800*2);
                    }
                    else if(!lk_fl) //当此根线不是文件的第一根线时
                    {
                        if(decAngle_last == decAngle) //当此时的角度与上次的角度一致时
                        {
                            count_to_ten++; //在一个角度十根线的计数上加一
                            if(count_to_ten<=10) //当此角度的线数不超过十根时
                            {
                                lk_c++;
                                lk_buffer[(lk_c-1)*802] = count_to_ten; //将此数据线是当前角度的第几根线的数值存入
                                lk_buffer[(lk_c-1)*802+1] = decAngle; //将此时的坐标输入
                                //根据角度值判断是否属于计算区域内的值
                                if(angle_start < angle_end)
                                {
                                    if(angle_start <= decAngle && decAngle <= angle_end)
                                    {
                                        figure_read_data << decAngle + 0.1*(count_to_ten-1); //将double类型的角度存入与计算线程交换的内存中
                                        for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                        {
                                            figure_read_data << decRadial[j];
                                        }
                                    }
                                }
                                else if(angle_start > angle_end)
                                {
                                    if(decAngle <= angle_end || decAngle >= angle_start)
                                    {
                                        figure_read_data << decAngle + 0.1*(count_to_ten-1); //将double类型的角度存入与计算线程交换的内存中
                                        for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                        {
                                            figure_read_data << decRadial[j];
                                        }
                                    }
                                }
                                memcpy(&lk_buffer[(lk_c-1)*802+2],decRadial,800*2);
                            }
                        }
                        else if(decAngle_last != decAngle)
                        {
                            if(count_to_ten < 10) //若上一个角度的线数不够10根时
                            {
                                for(int j=0;j<(10-count_to_ten);j++) //循环次数为上一个角度缺的数据线数
                                {
                                    lk_c++;
                                    lk_buffer[(lk_c-1)*802] = count_to_ten + j + 1; //存入当前的线数
                                    lk_buffer[(lk_c-1)*802+1] = decAngle_last; //将上一个角度值存入
                                    //根据角度值判断是否属于计算区域内的值
                                    if(angle_start < angle_end)
                                    {
                                        if(angle_start <= decAngle_last && decAngle_last <= angle_end)
                                        {
                                            figure_read_data << decAngle_last + 0.1*(count_to_ten-1); //将double类型的角度存入与计算线程交换的内存中
                                            for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                            {
                                                figure_read_data << decRadial[j];
                                            }
                                        }
                                    }
                                    else if(angle_start > angle_end)
                                    {
                                        if(decAngle_last <= angle_end || decAngle_last >= angle_start)
                                        {
                                            figure_read_data << decAngle_last + 0.1*(count_to_ten-1); //将double类型的角度存入与计算线程交换的内存中
                                            for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                            {
                                                figure_read_data << decRadial[j];
                                            }
                                        }
                                    }
                                    memcpy(&lk_buffer[(lk_c-1)*802+2],decRadial,800*2);
                                }
                            }
                            //此时应判断是否一次读满///////////////////////////////////////////
                            if(angle_count%60 == 0) //当数据线读够60度时，一次读取完毕
                            {
                                if(longkou_firsttime_swap)
                                {
                                    longkou_firsttime_swap = false;
                                    global::startreview_clicked = true; //开启主线程的画图
                                    global::run_firsttime = true;
                                    qDebug() << "开启opengl中的变量";
                                    //emit start_showing(); //发送信号，通知playback中开启绘制线程中的变量
                                }
                                qint16 tem[802*600];
                                memcpy(tem,lk_buffer,802*600*2);
                                emit pb_paint(tem);
                                lk_c = 0; //将用来对数组进行定位的变量重置为零
                            }
                            if(angle_count == 360)
                            {
                                angle_count = 0;
                                count_frame++; //用于记录当前是第几帧数据
                                nt = count_frame; //时间维数，当前是第几帧的数据
                                count_line = 0;
                                for(int i=1;i<=ny;i++)
                                {
                                    //点数×径向方向上点之间的距离，目前暂用derta(x)作为点之间的间隔
                                    y[i] = (radial_start_para + i)*sample[5]; //将计算区域内的极坐标半径存入数组
                                }

                                for(int i=0,count=0;i<nx*(ny+1);i=i+1+ny,count++)
                                {
                                    x[count+1] = figure_read_data[i]; //取出全局容器中的角度
                                    count_line++;
                                    for(int j=1;j<=ny;j++)
                                    {
                                        z[j+(count_line-1)*ny] = figure_read_data[count*(1+ny)+j]; //将一个角度上的强度值存入数组
                                    }
                                }
                                if(dll_isload) //当动态链接库加载成功时
                                {
                                    if(interfun)
                                    {
                                        qDebug() << "函数调用成功";
                                        interfun(x,y,z,sample,nx,ny,nt,dataOut,begin,end,2010); //调用第一个函数，输出为dataOut
                                        //查看dataOut
                                        figure_read_data.clear();
                                    }
                                    else
                                    {
                                        qDebug() << "函数调用失败";
                                    }
                                }
                                if(nt == int(sample[1])) //当帧数达到Nt时
                                {
                                    if(invers_calculation)
                                    {
                                        qDebug() << "波高计算函数调用成功";
                                        invers_calculation(sample,dataOut,begin,end,dataResult); //调用第二个函数，输出为dataResult
                                        double fre_cor[128];
                                        double wave_spe[128];
                                        double wave_dir[128];
                                        double wavenum_fre[128*128];
                                        for(int i = 0; i<128; i++)
                                            fre_cor[i] = dataResult[51+i];
                                        for(int i = 0; i<128; i++)
                                            {
                                            wave_spe[i] = dataResult[51+128+i];
                                            qDebug() << wave_spe[i];
                                        }

                                        for(int i = 0; i<128; i++)
                                            wave_dir[i] = dataResult[51+128*2+i];
                                        for(int i=0; i<128*128; i++)
                                            wavenum_fre[i] = dataResult[51+128*4+128*128*64];
                                        ////计算完毕后向playback发送信号，用于向主线程中发送信号，以改变主界面上的最终计算结果
                                        emit playback_figure_returnvalue(dataTime,dataResult[1],dataResult[1],dataResult[2],dataResult[4],dataResult[5],dataResult[6],dataResult[7],fre_cor,wave_spe,wave_dir,wavenum_fre);
                                    }
                                }
                            }
                            if(decAngle_last < decAngle) //当上次的角度比现在记录的角度小时
                            {
                                angle_count = angle_count + decAngle - decAngle_last; //角度数增加两个角度之间的差
                            }
                            else if(decAngle_last > decAngle)
                            {
                                angle_count = angle_count + (360 - decAngle_last) + decAngle; //角度数增加两个角度之间的差
                            }
                            ///////////////////////////////////////////////////////////////
                            count_to_ten = 1; //补完上一个角度的数据后，将计数设为1
                            decAngle_last = decAngle; //将当前的角度存入
                            lk_c++;
                            lk_buffer[(lk_c-1)*802] = count_to_ten; //将当前角度下的线数的计数存入
                            lk_buffer[(lk_c-1)*802+1] = decAngle; //将当前的角度存入缓存
                            //根据角度值判断是否属于计算区域内的值
                            if(angle_start < angle_end)
                            {
                                if(angle_start <= decAngle && decAngle <= angle_end)
                                {
                                    figure_read_data << decAngle + 0.1*(count_to_ten-1); //将double类型的角度存入与计算线程交换的内存中
                                    for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                    {
                                        figure_read_data << decRadial[j];
                                    }
                                }
                            }
                            else if(angle_start > angle_end)
                            {
                                if(decAngle <= angle_end || decAngle >= angle_start)
                                {
                                    figure_read_data << decAngle + 0.1*(count_to_ten-1); //将double类型的角度存入与计算线程交换的内存中
                                    for(int j = radial_start_para+1;j<=radial_end_para;j++)
                                    {
                                        figure_read_data << decRadial[j];
                                    }
                                }
                            }
                            memcpy(&lk_buffer[(lk_c-1)*802+2],decRadial,800*2);
                        }
                    }
                }
            }
            if(file1.atEnd()) //若此文件已读完，则返回
            {
                return;
            }
            file1.close(); //当读取到文件尾部时，关闭文件
        }

        if(data_format == "平潭数据格式") //当选定的数据格式为“平潭数据格式”时
        {
            pingtan_firsttime_swap = true;
            figure_read_data.clear(); //将存储计算数据的容器清零
            file1.setFileName(filename_text); //设置文件的名称
            file1.open(QIODevice::ReadWrite); //打开文件，设置为只读模式
            streamIn.setDevice(&file1); //设置数据流读入的设备file1
            pingtan_count_line = 0; //用于记录当前是当前帧的第几根线
            pt_count_forten = 0;
            pt_angle = 0;
            pt_frame = 0;
            //开始读取文件的头信息
            data = file1.read(100).toHex(); //读取前100位的数据
            //采样帧数
            sample_frames_int = data.mid(78,8).left(2).toInt(&ok,16);
            qDebug() << "采样帧数" << sample_frames_int;
            //每条线的采样数
            sample_per_line_int = data.mid(86,8).left(2).toInt(&ok,16)+data.mid(86,8).left(4).right(2).toInt(&ok,16)*256;
            qDebug() << "每条线的采样数" << sample_per_line_int;
            //角度分辨率
            /*
            angular_resolution.clear(); //先清空，第二次及之后回放时才不会出错
            angular_resolution.append(data.mid(94,8).right(2));
            angular_resolution.append(data.mid(94,8).right(4).left(2));
            angular_resolution.append(data.mid(94,8).left(4).right(2));
            angular_resolution.append(data.mid(94,8).left(2));
            angular_resolution_int = angular_resolution.toInt(&ok,16);
            angular_resolution_float = *(float*)&angular_resolution_int;*/
            //采样起始距离
            sample_start_distance_int = data.mid(102,8).left(2).toInt(&ok,16) + data.mid(102,8).left(4).right(2).toInt(&ok,16)*256;
            qDebug() << "采样起始距离" << sample_start_distance_int;
            //采样结束距离
            sample_end_distance_int = data.mid(110,8).left(2).toInt(&ok,16)+data.mid(110,8).left(4).right(2).toInt(&ok,16)*256;
            qDebug() << "采样结束距离" << sample_end_distance_int;

            //将读取到的采样起始距离和采样结束距离存入初始文件中
            ini->setValue("/minimum_observation_radius/minimum",QString::number(sample_start_distance_int));
            ini->setValue("/maximum_observation_radius/maximum",QString::number(sample_end_distance_int));
            //emit pingtan_change_distance(); //向playback中发送消息，使其向主界面发送消息改变显示观测范围
            pingtan_count_line = 0; //用于记录当前是当前帧的第几根线,先初始化为0
            pingtan_count_one_frame = 3600; //暂时
            //pingtan_count_one_frame = 360/angular_resolution_float; //根据角度分辨率算出一帧中所对应的线数
            pt_buffer = new qint16[600*sample_per_line_int]; //建立数组的大小
            decData = new qint16[sample_per_line_int]; //存入一根线的数据的数组
            //同时具有playback类中设为真和opengl类中设为真
            //当文件未读完时
            while(!file1.atEnd() && pt_frame < sample_frames_int) //当绘制部分在读取数据时，在此while中循环
            {
                data = file1.read(1).toHex(); //一位一位的读取
                if(data == "01") //当找到标志位后
                {
                    pingtan_count_line++; //用于记录当前是当前帧的第几根线
                    pt_count_forten++;
                    if(pt_count_forten == 10)
                    {    pt_angle++;
                         pt_count_forten = 1;
                    }
                    streamIn.readRawData((char*)decData,2*sample_per_line_int); //将一根线上的数据一次性读入
                    //将存入数组的数据存入全局容器中
                    for(int i=0;i<sample_per_line_int;i++)
                    {
                        pt_buffer[i+(pingtan_count_line-1)*sample_per_line_int] = decData[i];
                    }
                    if(angle_start < angle_end)
                    {
                        if(angle_start <=  pt_angle &&  pt_angle <= angle_end)
                        {
                            figure_read_data <<  pt_angle + 0.1*(pt_count_forten-1); //将double类型的角度存入与计算线程交换的内存中
                            for(int j = radial_start_para+1;j<=radial_end_para;j++)
                            {
                                figure_read_data << decData[j];
                            }
                        }
                    }
                    else if(angle_start > angle_end)
                    {
                        if( pt_angle <= angle_end ||  pt_angle >= angle_start)
                        {
                            figure_read_data <<  pt_angle + 0.1*(pt_count_forten-1); //将double类型的角度存入与计算线程交换的内存中
                            for(int j = radial_start_para+1;j<=radial_end_para;j++)
                            {
                                figure_read_data << decData[j];
                            }
                        }
                    }
                    qDebug() << "pt_angle" << pt_angle;
                    if(pt_angle == 360)
                    {
                        pt_angle = 0;
                        pt_frame++; //用于记录当前是第几帧数据
                        nt = pt_frame; //时间维数，当前是第几帧的数据
                        count_line = 0;
                        for(int i=1;i<=ny;i++)
                        {
                            //点数×径向方向上点之间的距离，目前暂用delta(x)作为点之间的间隔
                            y[i] = (radial_start_para + i)*sample[5]; //将计算区域内的极坐标半径存入数组
                        }
                        qDebug() << "Y数组已经存储好";
                        for(int i=0,count=0;i<nx*ny;i=i+1+ny,count++)
                        {
                            x[count+1] = figure_read_data[i]; //取出全局容器中的角度
                            count_line++;
                            for(int j=1;j<=ny;j++)
                            {
                                z[j+(count_line-1)*ny] = figure_read_data[count*(1+ny)+j]; //将一个角度上的强度值存入数组
                            }
                        }
                        if(dll_isload) //当动态链接库加载成功时
                        {
                            if(interfun)
                            {
                                qDebug() << "函数调用成功";
                                interfun(x,y,z,sample,nx,ny,nt,dataOut,begin,end,2010); //调用第一个函数，输出为dataOut
                                //查看dataOut
                            }
                            else
                            {
                                qDebug() << "函数调用失败";
                            }
                        }
                        if(nt == int(sample[1])) //当帧数达到Nt时
                        {
                            if(invers_calculation)
                            {
                                qDebug() << "波高计算函数调用成功";
                                invers_calculation(sample,dataOut,begin,end,dataResult); //调用第二个函数，输出为dataResult
                                ////计算完毕后向playback发送信号，用于向主线程中发送信号，以改变主界面上的最终计算结果
                                //emit playback_figure_returnvalue(dataResult[4],dataResult[5],dataResult[6],dataResult[7]);
                            }
                        }
                    }
                    if(pingtan_count_line == 600)
                    {
                        pingtan_count_line = 0; ////将记录当前是第几根线的变量置为0
                        if(pingtan_firsttime_swap)
                        {
                            pingtan_firsttime_swap = false;
                            global::startreview_clicked = true; //开启主线程的画图
                            global::run_firsttime = true;
                            //emit start_showing(); //发送信号，通知playback中开启绘制线程中的变量
                        }
                        qint16 tem[600*sample_per_line_int];
                        memcpy(tem,pt_buffer,600*sample_per_line_int*2);
                        //将数据发送给opengl
                        emit pt_paint(pingtan_count_one_frame,sample_per_line_int,sample_start_distance_int,sample_end_distance_int,tem);
                    }
                }
            }
            if(file1.atEnd()) //若此文件已读完，则返回
            {
                return;
            }
            file1.close();
            qDebug() << "文件关闭";
            delete []pt_buffer;
            delete []decData;
        }
        delete []x;
        delete []y;
        delete []z;
        delete []dataOut;
        delete []dataResult;
        delete []tempData;
        delete []tempData1;
        delete []tempData2;
        delete []RRAW_buffer;
        delete []cal_buffer;
        dll.unload(); //当此线程销毁时，解绑动态链接库
        qDebug() << "线程运行完毕";
}


