#include "startsavingtoharddisk.h"

startsavingtoharddisk::startsavingtoharddisk(uchar *&save_buffer,float *&angle_buff,QReadWriteLock *share_lock,QObject*parent):
    QObject(parent),buf_read(save_buffer),angle_buf(angle_buff),lock_read(share_lock)
{
}

startsavingtoharddisk::~startsavingtoharddisk()
{
}

//将数据存入硬盘的函数
void startsavingtoharddisk::harddisk_save_thread()
{
    /*
    while(global::start_running_thread)
    {
    }*/
    qDebug() << "startsavingtoharddisk函数被调用";
}

//接收来自paintonline线程的经纬度，用于数据文件头的存储
void startsavingtoharddisk::gps_toheadSave_Slot(int *data)
{
    memcpy(gps_headData,data,2*4);
    qDebug() << "经度和纬度" << gps_headData[0] << gps_headData[1];
}

//循环体中要用的内容
void startsavingtoharddisk::harddisk_save_thread2(double *ins_Data)
{
    //qDebug() << "startsavingtoharddisk线程开始上锁";
    lock_read->lockForRead();
    //qDebug() << "startsavingtoharddisk线程上锁成功";
    memcpy(insData,ins_Data,16*10*N*sizeof (double)); //将paintonline传输的数据拷贝下来
    //qDebug() << "startsavingtoharddisk线程的循环体被调用";
    if(for_first_run)
    {
        for_first_run = false;
        emit start_saving_message(); //这里的内容，每次点击“开始处理”（点击停止处理时不会）运行一次

        //第一次点击“开始处理”后，运行一次与共享内存有关的设置   `
        //从初始文件中获取存储路径
        //此路径一定存在，在写入ini文件时已进行确认
        //放在第一次运行的判断外，为了防止“暂停处理”时改变存储路径和文件夹
        QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        int min = ini->value("/radar_information/minimum").toInt();
        int max = ini->value("/radar_information/maximum").toInt();
        float sample_x = ini->value("/radar_information/samplestep").toFloat();
        sample_num = int((max - min) / sample_x); //通过观测范围和采样间距计算出采样点数
        QString path_ini = ini->value("/initialPath/savepath").toString(); //获取存储路径
        QString site_ini = ini->value("/site_information/name").toString(); //获取站点名称
        frame_num = ini->value("/collection_mode/framenumber").toInt(); //获取每隔数据文件存储的数据总帧数
        QDateTime time = QDateTime::currentDateTime(); //获取当前系统的时间
        QString year = time.toString("yyyy") + "年"; //获取当前的年份
        QString dir_name = year + site_ini; //文件夹名称为年份加站点名称
        //qDebug() << "文件夹名称为年份加时间" << dir_name << "num = " << sample_num;

        QDir data_save_dir;
        if(!data_save_dir.exists(path_ini))
        {
            qDebug() << "运行设置中的存储路径不存在！";
            path_ini = QCoreApplication::applicationDirPath();
        }
        data_save_dir.setCurrent(path_ini); //设置现在的目录为存储在ini文件中的目录
        //qDebug() << "文件夹所在的目录" << path_ini;
        if(!data_save_dir.exists(dir_name)) //判断在此路径下是否存在指定文件夹
        {
            qDebug() << "文件不存在，新建一个文件";
            data_save_dir.mkdir(dir_name); //不存在时，生成文件夹
        }
        QString year_dir_path = data_save_dir.filePath(dir_name); //获取“年份+地点”文件夹的路径
        data_save_dir.setCurrent(year_dir_path); //将“年份+地点”文件夹的路径设为当前路径
        QString month = time.toString("M") + "月"; //文件夹名称为系统时间的月份
        if(!data_save_dir.exists(month)) //判断此路径下是否存在现在月份的文件夹
        {
            data_save_dir.mkdir(month); //不存在时，生成子文件夹
        }
        data_save_dir.setCurrent(month); //设置当前月份对应的文件夹路径为当前路径
        filename = time.toString("yyyy_MM_dd_hh_mm_ss")+".RRAW";
        file.setFileName(filename);
        if(!file.exists()) //判断此文件现在是否存在，不存在时新建
        {
            qDebug() << "文件已经存在";
            file.open(QIODevice::ReadWrite);
            file.close();
        }
        streamIn.setDevice(&file);
        //qDebug() << "saving调用一次数据头函数11";
        filehead_Slot(); //数据文件写入文件头数据
        saveAngle_old = angle_buf[0] - 0.1;
        //qDebug() << "startsavingtoharddisk线程中初始化的角度值为" << saveAngle_old << angle_buf[0]-10;
    }
    if(file.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        //QTime time;
        //time.start();
        //qDebug() << "startsavingtoharddisk线程中" << saveAngle_old << angle_buf[0];
        if(angle_buf[0] >= saveAngle_old && angle_buf[0] - saveAngle_old < 0.2)
        {
            //qDebug() << "startsavingtoharddisk线程中开始存入数据1";
            for(int i=0;i<10*N;i++)
            {
                //qDebug() << "startsavingtoharddis存入1";
                file.write(QByteArray::fromRawData((char *)insData,16*sizeof (double)));
                //qDebug() << "startsavingtoharddis存入2";
                //file.write(QByteArray::fromRawData((char *)markerBit,1));
                streamIn << markerBit;
                //qDebug() << "startsavingtoharddis存入3";
                file.write(QByteArray::fromRawData((char *)buf_read,sample_num*2));
                //qDebug() << "startsavingtoharddis存入4";
            }
        }
        else if(angle_buf[0] < saveAngle_old && 360 - saveAngle_old + angle_buf[0] < 0.2)
        {
            //qDebug() << "startsavingtoharddisk线程中开始存入数据2";
            for(int i=0;i<10*N;i++)
            {
                //qDebug() << "startsavingtoharddis存入1";
                file.write(QByteArray::fromRawData((char *)insData,16*sizeof (double)));
                //qDebug() << "startsavingtoharddis存入2";
                //file.write(QByteArray::fromRawData((char *)markerBit,1));
                streamIn << markerBit;
                //qDebug() << "startsavingtoharddis存入3";
                file.write(QByteArray::fromRawData((char *)buf_read,sample_num*2));
                //qDebug() << "startsavingtoharddis存入4";
            }
        }
        /*else
        {
            qDebug() << "startsavingtoharddisk线程中开始存入数据3";
            //char bitTemp[int((angle_buf[0] - saveAngle_old)*10)];
            //for(int m=0;m<int(angle_buf[0] - saveAngle_old)*10-1;m++)
                    //bitTemp[m] = 0;
            if(angle_buf[0] > saveAngle_old)
            {
                int n = int((angle_buf[0] - saveAngle_old)*10-1)*int(2*sample_num + 1 + 16*sizeof (double));
                qDebug() << "存储硬盘中N1的值为" << angle_buf[0] << saveAngle_old << n;
                for(int k=0;k<n;k++)
                    streamIn << 0x00;
            }
            else
            {
                int n = int((360 - saveAngle_old + angle_buf[0])*10-1)*int(2*sample_num + 1 + 16*sizeof (double));
                qDebug() << "存储硬盘中N2的值为" << angle_buf[0] << saveAngle_old << n;
                for(int k=0;k<n;k++)
                    streamIn << 0x00;
            }
            //streamIn << bitTemp;
            //file.write(QByteArray::fromRawData((char *)bitTemp,int((angle_buf[0] - saveAngle_old)*10)));
            for(int i=0;i<10*N;i++)
            {
                file.write(QByteArray::fromRawData((char *)insData,16*sizeof (double)));
                streamIn << markerBit;
                file.write(QByteArray::fromRawData((char *)buf_read,sample_num*2));
            }
        }*/
        lock_read->unlock();
        saveAngle_old = angle_buf[10*N-1];
        count_for_frame+=10*N; //用以判断什么时候存满一组数据
        //qDebug() << "startsavingtoharddisk线程中将数据存入硬盘完成";
        file.close();
    }

    if(count_for_frame == 3600*frame_num) //当存满一组数据的时候
    {
        count_for_frame = 0;
        QDateTime time_now = QDateTime::currentDateTime(); //获取当前系统的时间
        filename = time_now.toString("yyyy_MM_dd_hh_mm_ss")+".RRAW";
        file.setFileName(filename);
        if(!file.exists()) //判断此文件现在是否存在，不存在时新建
        {
            //qDebug() << "文件已经存在";
            file.open(QIODevice::ReadWrite);
            file.close();
        }
        //qDebug() << "saving调用一次数据头函数12";
        filehead_Slot(); //数据文件写入文件头数据
    }
    //qDebug() << "startsavingtoharddisk线程的循环体调用完毕";
}

//数据文件写入文件头数据
void startsavingtoharddisk::filehead_Slot()
{
    qDebug() << "saving调用一次数据头函数2";
    //在此处向文件中输入前100个字节的数据
    if(file.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        char head[100] = {0};
        //软件版本号、保存时间、站点名称共31个字节
        QString site_num = "MRDV1.1"; //临时
        //软件版本号：7个字节
        streamIn << site_num; //存入软件版本号
        //保存时间：14个字节
        streamIn << QDateTime::currentDateTime().toString("yyyyMMddhhmmss"); //存入当前的保存时间
        QSettings *setting = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        QString sitename = setting->value("/site_information/name").toString(); //读出站点名称

        //站点名称：10个字节
        if(sitename.length() <= 10)
        {
            streamIn << sitename;
            for(int i=0;i < 10 - sitename.length();i++)
                streamIn << 0x00;
        }
        else if(sitename.length() > 10)
        {
            for(int i=0;i<10;i++)
                streamIn << 0x00;
            QMessageBox::warning(nullptr,"站点名称存储错误","站点名称长度过长，超过10个字符，无法存入文件中！",nullptr,QMessageBox::Ok);
        }
        //经度、纬度:4个字节
        streamIn << gps_headData[0] << gps_headData[1];
        //Nt,int类型
        /////////////
        file.write(head,4);
        //每根线上的采样点数:4个字节
        streamIn << sample_num;
        //角度分辨率,float类型，一般为CD CC CC 3D
        streamIn << 0xCD << 0xCC << 0xCC << 0x3D;
        //起始距离、结束距离,int类型
        streamIn << min << max;
        //采样频率,float类型
        file.write(head,4);
        //修正角度:4个字节
        streamIn << setting->value("/calculation_parameter/radar_correction_angle").toInt();
        //一个标志位
        //////////////
        streamIn << 0x01;
        //计算区域的四个边界值：8个字节
        qint16 value;
        value = setting->value("/area/angle1").toInt();
        streamIn << value;
        value = setting->value("/area/angle2").toInt();
        streamIn << value;
        value = setting->value("/area/radio1").toInt();
        streamIn << value;
        value = setting->value("/area/radio2").toInt();
        streamIn << value;

        int frame = setting->value("/collection_mode/framenumber").toInt(); //读出每个数据文件存储的帧数
        streamIn << frame;
        float n = setting->value("/calculation_parameter/sampling_interval_time").toFloat(); //读出时间抽样间隔，即周期
        streamIn << float(1/n); //计算出转速，存入数据头中
        for(int i=0;i < 24;i++)
            streamIn << 0x00;
    }
    file.close();
}
