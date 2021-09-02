#include "paintonline.h"
#include <QTime>

//QVector<unsigned __int16>buffer_show_processed; //定义全局容器，用于显示子线程和显示主线程中交换使用，此容器为子线程写入数据使用
//QVector<unsigned __int16>buffer_show_processed_mainthread; //定义全局容器，用于显示子线程和显示主线程中交换使用，此容器为主线程读数用
//QVector<double>buffer_figure;
//QMutex mutex;
//QVector<double>a_buffer;
//QVector<double>d_buffer;

//qint16 buf[80100];

paintonline::paintonline(uchar *&share_buffer,float *&angle_Buffer,QReadWriteLock *share_lock,double *&insData,QMutex *insMutex,double *&figurexData,double *&figurezData,QMutex *figureMutex,uchar *&saveData,float *&saveAngle,QReadWriteLock *saveMutex,QObject *parent):
    QObject(parent),buf_read(share_buffer),angle_Buffer(angle_Buffer),lock_read(share_lock),insData(insData),insMutex(insMutex),figurexData(figurexData),figurezData(figurezData),figureMutex(figureMutex),saveData(saveData),saveAngle(saveAngle),saveMutex(saveMutex)
{
}

void paintonline::paintonline_thread()
{
    qDebug() << "调用了paintonline_thread()函数";
    emit paint_online_message(); //发送信号回主线程以告知此线程开启成功
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    frameTotal = ini->value("/collection_mode/framenumber").toInt(); //读出一组数包含的帧数
    state = ini->value("/radar_information/state").toBool(); //状态变量设置
    //int min = ini->value("/radar_information/minimum").toInt();
    //int max = ini->value("/radar_information/maximum").toInt();
    int min = ini->value("/radar_information/startpoint").toInt();
    int max = ini->value("/radar_information/endpoint").toInt();
    float sample_x = ini->value("/radar_information/samplestep").toFloat();
    num_paint = (max - min) / sample_x; //通过观测范围和采样间距计算出采样点数(触发延时之后的点数改变？)
    qDebug() << "paintonline中的num值为" << num_paint;
    qDebug() << "paintonline采样起始距离和采样结束距离和采样点数" << max << min << sample_x;
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
    //采样步长
    float sampleStep = ini->value("/radar_information/samplestep").toFloat();
    start_count_radial = startradial/sampleStep; //计算区域在一个角度上，800个点中，起始的点数
    end_count_radial = endradial/sampleStep; //计算区域在一个角度上，800个点中，截止的点数
    if(endangle > startangle)
    {
        nx = (endangle - startangle)*10; //nx = (angle_end - angle_start)*10;
    }
    else if(endangle <= startangle)
    {
        nx = (360 - startangle + endangle)*10; //nx = (360 - angle_start + angle_end)*10;
    }
    //data_count = figure_calculate_data.length();  //获取数组的长度
    ny = end_count_radial - start_count_radial; //在当前选定的计算区域内，存下一个角度上的点数
    //nx = data_count/(ny+1); //得到极坐标中角度的总个数
    //nx = 3600;
    nt = 0; //将当前回放的帧数置零
    //figurexData = new double[nx+1]; //新建动态数组，用于存放当前计算区域下所有的极坐标角度
    figurezTemp = new double[nx*ny+1]; //新建动态数组，用于存放当前计算区域下所有的强度值，数组大小和存储极坐标半径的数组大小一致
    angle_cor = ini->value("/calculation_parameter/radar_correction_angle").toDouble(nullptr); //旋转的角度
    ins = ini->value("/ins_com/check").toInt(); //判断是否有惯导输入
    out = ini->value("/out_com/check").toInt(); //是否需要输出计算结果
    //选择岸基或者船载
    if(ini->value("/site_information/onBoard").toInt())
        isBoard = true;
    else if(ini->value("/site_information/shoreBased").toInt())
        isBoard = false;
    qDebug() << "paintonline线程的初始函数运行完毕";
}

//循环体中的内容
void paintonline::paintonline_thread2(int angle_Count)
{
    lock_read->lockForRead(); //雷达采集数据
    qDebug() << "paintonline_thread2中收到的angle_Count为" << angle_Count;
    figureMutex->lock(); //雷达计算数据
    saveMutex->lockForWrite(); //雷达存储数据
    qDebug() << "paintonline_Lock";
    test_count++; //每运行一次加一，角度增加10度
    nt = (test_count*N)/360; //计算出当前是第几帧
    //qDebug() << "paintonline_thread2中计算出当前帧数为" << nt;
    //qDebug() << "paintonline_thread2中所有的线程锁已经上锁";
    int angleCount = angle_Count; //存入一共的角度数量
    float angleBuffer[angleCount]; //新建一个数组，用于存储所有角度
    memcpy(&angleBuffer[0],&angle_Buffer[0],angleCount*sizeof(float)); //将数组拷入此线程的内存中
    double angle_sea = 0; //正在查找的角度值
    QString angle_str = QString::asprintf("%.1f",angleBuffer[0]); //将第一个角度值按照四舍五入保留一位小数
    qDebug() << "paintonline中的" << angleCount << ins << isBoard;
    float angle_flo = angle_str.toFloat(); //提取角度数组中的第一个值，保留一位小数后转为float
    qint16 a_a[(num_paint+1)*10*N]; //发送给openglwidget主线程的数组
    //qint16 a_a[(400+1)*10*10]; //发送给openglwidget主线程的数组
    double insforLine[N*10*16]; //发送给startsavingtoharddisk子线程的数组
    global::ins_start = false; //将惯导中的循环停掉，以便对惯导数据进行读取
    qDebug() << "paintonline中1";
    //惯导数据的处理
    if(ins && isBoard)
    {
        //qDebug() << "。。。。。。进入惯导数据处理";
        insMutex->lock();
        //insNum = (((N*10.0)/insData[0])*10+5)/10; //每多少根线赋一个惯导数据值,四舍五入
        insNum = (N*10.0)/insData[0]; //每多少根线赋一个惯导数据值,未四舍五入
        //qDebug() << "拷入惯导数据" << insData[0] << insData[10] << insData[11];
        memcpy(&insTemp,&insData[1],insData[0]*16*sizeof(double)); //将惯导数据拷贝出来
        global::ins_start = true; ////0925
        insMutex->unlock();
        for(int insAngleNum = 0;insAngleNum < int(insData[0]);insAngleNum++)
        {
            for(int insTranNum = int(insAngleNum*insNum);insTranNum < int((insAngleNum+1)*insNum);insTranNum++)
            {
                memcpy(&insforLine[insTranNum*16],&insTemp[insAngleNum*16],16*sizeof(double));
            }
        }
        int insAngleDiff = N*10 - int(int(insData[0])*insNum);
        if(insAngleDiff > 0)
        {
            for(int insTranNum = int(insData[0])*insNum+1; insTranNum <10*N;insTranNum++)
                memcpy(&insforLine[insTranNum*16],&insTemp[int(insData[0]-1)*16],16*sizeof(double));
        }
        /*
        for(int insAngleNum = 0;insAngleNum < (10*N)/insNum;insAngleNum++)
        {
            for(int insTranNum = int(insAngleNum*insNum);insTranNum < int((insAngleNum+1)*insNum);insTranNum++)
            {
                memcpy(&insforLine[insTranNum*16],&insTemp[insAngleNum*16],16*sizeof(double));
            }
        }*/
    }
    else
    {
        for(int i=0;i<N*10*16;i++) //岸基条件下，惯导值赋0
            insforLine[i] = 0;
    }

    if(isFirst)
    {
        angleLast = angle_str.toDouble();
        //testCountOld = test_count;
        isFirst = false;
    }

    double insoutTemp[5] = {0}; //用于计算使用的惯导数组，用于存储320.0度时的惯导值
    //qint16 temp[num_paint*10*N];
    qDebug() << "paintonline中2";
    for(int i=0;i<10*N;i++) //找出每个角度值对应的位置
    {
        //qDebug()<<"====================="<<i;
        qDebug() << "paintonline中31";
        int low = 0; //最左边的位置
        int high = angleCount - 1; //最右边的位置
        int mid = 0; //中间的位置
        angle_sea = angle_flo + 0.1*i;
        while(low <= high)
        {
            mid = (low + high)/2;
            if(abs(angleBuffer[mid+1]-angle_sea) > abs(angleBuffer[mid]-angle_sea))
            {
                high = mid - 1;
            }
            else
            {
                low = mid + 1;
            }
        }
        int angle_pos = abs(angleBuffer[mid+1]-angle_sea)>abs(angleBuffer[mid]-angle_sea)?mid:(mid+1); //此时对应角度angle_sea所在的位置
        /*if(angle_pos >= 2)
        {
            qDebug() << "paintonline中32";
            int count = 0;
            int start_pos = 0;
            bool first_pos = true;

            for(int j = angle_pos - 2; j < angle_pos + 3;j++)
            {
                if(abs(angleBuffer[j] - angle_sea) < 0.05)
                {
                    if(first_pos)
                    {
                        first_pos = false;
                        start_pos = j;
                    }
                    count++;
                }
            }
            if(count == 0)
            {
                qint16 temp1[num_paint];
                qint16 temp2[num_paint];
                if(angleBuffer[angle_pos] > angle_sea)
                {
                    memcpy(&temp1[i*num_paint],&buf_read[(angle_pos-1)*(2*num_paint+40)],2*num_paint*sizeof (uchar));
                    memcpy(&temp2[(i+1)*num_paint],&buf_read[(angle_pos+1)*(2*num_paint+40)],2*num_paint*sizeof (uchar));
                }
                else
                {
                    memcpy(&temp1[i*num_paint],&buf_read[(angle_pos-1)*(2*num_paint+40)],2*num_paint*sizeof (uchar));
                    memcpy(&temp2[(i-1)*num_paint],&buf_read[(angle_pos+1)*(2*num_paint+40)],2*num_paint*sizeof (uchar));
                }
                for(int m=0;m<num_paint;m++)
                {
                    temp[i*num_paint+m] = (temp1[i*num_paint+m] + temp2[i*num_paint+m])/2;
                }
            }
            else
            {
                qint16 data[count][num_paint];
                qint16 temp[num_paint];
                for(int j=start_pos;j<start_pos+count;j++)
                {
                    memcpy(&data[j-start_pos],&buf_read[j*(2*num_paint+40)],2*num_paint*sizeof (uchar));
                }
                for(int j=0;j<count;j++)
                {
                    for(int m = 0; m < num_paint;m++)
                    {
                        temp[m] += data[j][m];
                    }
                }
                for(int j=0;j<num_paint;j++)
                    temp[i*num_paint+j] = temp[i*num_paint+j]/count;
            }
        }*/
        angle_sea += angle_cor; //加入修正角度后的角度值
        if(angle_sea >= 360.0) //angle_sea等于360度时，在显示部分用0度表示，否则内存容易溢出
            angle_sea -= 360;
        else if(angle_sea < 0)
            angle_sea += 360;
        a_a[i*(num_paint+1)] = qint16(angle_sea*10);
        /*
        //用于存储的数组
        saveAngle[i] = float(angle_sea);
        qDebug() << "paintonline中33";
        //memcpy(&saveData[i*2*num_paint],&buf_read[angle_pos*(2*num_paint+40)],2*num_paint*sizeof (uchar)); //存入雷达数据
    }
    qDebug() << "paintonline中3";
    memcpy(&saveData[0],&temp[0],num_paint*10*N*sizeof (qint16));

    for(int i=0;i<10*N;i++)
    {
        angle_sea = angle_flo + 0.1*i;
        angle_sea += angle_cor; //加入修正角度后的角度值
        if(angle_sea >= 360.0) //angle_sea等于360度时，在显示部分用0度表示，否则内存容易溢出
            angle_sea -= 360;
        else if(angle_sea < 0)
            angle_sea += 360;
        a_a[i*(num_paint+1)] = qint16(angle_sea*10); //存入角度
        memcpy(&a_a[i*(num_paint+1)+1],&saveData[i*(2*num_paint+40)+40],2*num_paint*sizeof (uchar));
        /////////////////////////////////
        if(startangle < endangle)
        {
            if(startangle <= angle_sea && angle_sea < endangle)
            {
                //a_buffer.append(angle_start); //将double类型的角度存入与计算线程交换的内存中
                int figurePos = int((angle_sea - startangle)*10);
                figurexData[figurePos+1] = angle_sea;
                for(int j = start_count_radial+1;j <= end_count_radial;j++)
                {
                    figurezData[int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1] = a_a[i*(num_paint+1)+j+1];
                }
            }
        }
        else if(startangle > endangle)
        {
            if(angle_sea >= startangle)
            {
                int figurePos = int(angle_sea*10 - startangle*10);
                figurexData[figurePos+1] = angle_sea;
                for(int j = start_count_radial+1;j <= end_count_radial;j++)
                {
                    figurezData[int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1] = a_a[i*(num_paint+1)+j+1];
                }
            }
            else if(angle_sea < endangle)
            {
                int figurePos = int((3600 - startangle*10) + angle_sea*10);
                figurexData[figurePos+1] = angle_sea;
                //qDebug() << "paintonline中存入了计算数据3" << figurePos << angle_sea;
                for(int j = start_count_radial+1;j <= end_count_radial;j++)
                {
                    figurezData[int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1] = a_a[i*(num_paint+1)+j+1];
                }
            }
        }
        //获取320.0度时的惯导值
        if(int(angle_sea*10) == 3200)
        {
            if(ins)
            {
                insoutTemp[0] = sqrt(insforLine[(i*16)+6]*insforLine[(i*16)+6]+insforLine[(i*16)+8]*insforLine[(i*16)+8]); //由北速和东速计算出船速
                insoutTemp[1] = insforLine[(i*16)+13];
                insoutTemp[2] = insforLine[(i*16)+12];
                insoutTemp[3] = insforLine[(i*16)+14];
                insoutTemp[4] = insforLine[(i*16)+15];
            }
        }
    }
    */
        memcpy(&a_a[i*(num_paint+1)+1],&buf_read[angle_pos*(2*num_paint+40)+40],2*num_paint*sizeof (uchar)); //当前这个角度对应的线所在的地址
        //qDebug() << "paintonline中存入了惯导数据，其所在位置是" << 16*i;
        saveAngle[i] = float(angle_sea);
        //qDebug() << "paintonlie中用于存储的角度验证" << saveAngle[i];
        memcpy(&saveData[i*2*num_paint],&buf_read[angle_pos*(2*num_paint+40)],2*num_paint*sizeof (uchar)); //存入雷达数据

        //qDebug() << "paintonline中存入了当前的雷达数据和惯导数据" << i;
        //qDebug() << "paintonline线程中的信号发送完2毕";

        if(startangle < endangle)
        {
            if(startangle <= angle_sea && angle_sea < endangle)
            {
                //a_buffer.append(angle_start); //将double类型的角度存入与计算线程交换的内存中
                int figurePos = int((angle_sea - startangle)*10);
                figurexData[figurePos+1] = angle_sea;
                //qDebug() << "paintonline中存入了计算数据1" << figurePos << angle_sea;
                /*(for(int j = start_count_radial+1;j <= end_count_radial;j++)
                {
                    uchar char1[2];
                    char1[0] = saveData[i*2*num_paint+2*j+1];
                    char1[1] = saveData[i*2*num_paint+2*j];
                    double val = *(double *)char1;
                    //double val = 5444;
                    //qDebug() << "存入计算的数值" << val;
                    figurezTemp[int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1] = val;
                    figurezTemp[int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1] = a_a[i*(num_paint+1)+j+1];
                    //qDebug() << "paintonline中存入计算数据的位置" << int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1;
                }*/

                for(int j = start_count_radial+1;j <= end_count_radial;j++)
                {
                    figurezData[int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1] = a_a[i*(num_paint+1)+j+1];
                }
                //memcpy(&figurezTemp[int(figurePos*(end_count_radial-start_count_radial))+1],&a_a[i*(num_paint+1)+start_count_radial+1],(end_count_radial-start_count_radial)*sizeof (qint16));
                //qDebug() << "paintonline中存入了计算数据1_ending";
            }
        }
        else if(startangle > endangle)
        {
            if(angle_sea >= startangle)
            {
                int figurePos = int(angle_sea*10 - startangle*10);
                figurexData[figurePos+1] = angle_sea;
                //qDebug() << "paintonline中存入了计算数据2" << figurePos << angle_sea;
                /*for(int j = start_count_radial+1;j <= end_count_radial;j++)
                {
                    uchar char1[2];
                    char1[0] = saveData[i*(40+2*num_paint)+40+2*j+1];
                    char1[1] = saveData[i*(40+2*num_paint)+40+2*j];
                    double val = *(double *)char1;
                    //double val = 5444;
                    //memcpy(&val,&buf_read[i*(40+2*num_paint)+40+2*j],2*sizeof (uchar));
                    figurezTemp[int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1] = val;
                }*/
                for(int j = start_count_radial+1;j <= end_count_radial;j++)
                {
                    figurezData[int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1] = a_a[i*(num_paint+1)+j+1];
                }
                //memcpy(&figurezTemp[int(figurePos*(end_count_radial-start_count_radial))+1],&a_a[i*(num_paint+1)+start_count_radial+1],(end_count_radial-start_count_radial)*sizeof (qint16));
                //qDebug() << "paintonline中存入了计算数据2_ending";
            }
            else if(angle_sea < endangle)
            {
                int figurePos = int((3600 - startangle*10) + angle_sea*10);
                figurexData[figurePos+1] = angle_sea;
                //qDebug() << "paintonline中存入了计算数据3" << figurePos << angle_sea;
                for(int j = start_count_radial+1;j <= end_count_radial;j++)
                {
                    figurezData[int(figurePos*(end_count_radial-start_count_radial))+int(j-start_count_radial-1)+1] = a_a[i*(num_paint+1)+j+1];
                }
            }
        }
        //获取320.0度时的惯导值
        if(int(angle_sea*10) == 3200)
        {
            if(ins)
            {
                insoutTemp[0] = sqrt(insforLine[(i*16)+6]*insforLine[(i*16)+6]+insforLine[(i*16)+8]*insforLine[(i*16)+8]); //由北速和东速计算出船速
                insoutTemp[1] = insforLine[(i*16)+13];
                insoutTemp[2] = insforLine[(i*16)+12];
                insoutTemp[3] = insforLine[(i*16)+14];
                insoutTemp[4] = insforLine[(i*16)+15];
            }
        }
    }
    commonfunction common;
    if(state)
        common.CoCIRFun(10*N,num_paint,a_a);

    emit test_19252008(a_a,num_paint); //发送信号给主线程绘制图像

    if(global::waveshow)
        emit st_wave(num_paint+1,100,a_a); //发送信号至波形显示绘制图像

    saveMutex->unlock();
    qDebug() << "paintonline中发送至opengl";
    //qDebug() << "paintonline线程中的信号发送完毕2";
    emit save_harddisk_thread(insforLine); //发送信号到主线程调用存入硬盘的函数
    figureMutex->unlock();
    //qDebug() << "paintonline线程中的信号发送完毕3";

    qDebug() << "paintonline中发送计算的线程函数" << abs(angle_sea - angleLast) << test_count;
    //if(abs(angle_sea - angleLast) <= 0.1 && test_count != testCountOld)
    if(test_count%36 == 0)
    {
        angleLast = angle_sea;
        testCountOld = test_count;
        double insoutTemp[5] = {0};
        if(ins)
        {
            insoutTemp[0] = sqrt(insTemp[6]*insTemp[6]+insTemp[8]*insTemp[8]); //由北速和东速计算出船速
            insoutTemp[1] = insTemp[13];
            insoutTemp[2] = insTemp[12];
            insoutTemp[3] = insTemp[14];
            insoutTemp[4] = insTemp[15];
        }
        emit naviValue(int(insTemp[11]),int(insTemp[12]),insoutTemp[0],insTemp[13]); //经度纬度船速船向
        qDebug() << "............" << insTemp[11] << insTemp[12] << insoutTemp[0] << insTemp[13];
        emit frame(nt); //发送信号给主线程更新主界面上的当前帧数
        if(nt == frameTotal)
            test_count = 0;
        emit figurethread(insoutTemp); //发送信号给计算线程，进行一次数据计算
        qDebug() << "paintonline中发送计算的线程函数2" << nt;
    }
    qDebug() << "paintonline线程中的信号发送完毕";
    //在一帧计算的时候读入一次惯导数据
    //只有当有惯导数据输入时才成立

    //angle = angle_start*10 + angle_cor*10; //将修正角度加入，计算出真正角度值的十倍值，用于传给Opengl界面进行绘制
    /*if(angle > 3600) //当修正角度大于360度时
        angle = angle - 3600;
    else if(angle < 0) //当修正角度小于0度时
        angle = angle + 3600;*/
    /*if(out)
    {
        QByteArray byteout_data1,byteout_data2;
        byteout_data1.resize(4);
        byteout_data2.resize(4);
        memcpy(byteout_data1.data(),&dataResult[1],sizeof (dataResult[1]));
        memcpy(byteout_data2.data(),&dataResult[7],sizeof (dataResult[7]));
        serial2->write(byteout_data1);
        serial2->write(byteout_data2);
        qDebug() << "输出流速和有效波高" << dataResult[1] << dataResult[7];
    }*/
    lock_read->unlock();
}

paintonline::~paintonline()
{
    //delete [] figurexData;
    delete [] figurezTemp;
}
