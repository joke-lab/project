#include "startprocessing.h"

startprocessing::startprocessing(uchar *&share_buffer,float *&angleArray,QReadWriteLock *share_lock,QObject*parent):
    QObject(parent),buf_write(share_buffer),angleArray(angleArray),lock_write(share_lock) //share_buffer传入并使用
{
    qDebug() << "lock的地址" << share_lock;
    qDebug() << "lock_write的地址" << lock_write;
    //paint_online = new paintonline(buf,lock,nullptr);
    //save = new startSavingtoharddisk(buf,lock,nullptr);
    //connect(this,&startProcessing::preprocessing,this,&startProcessing::packet_grab_thread2);
}

void startprocessing::packet_grab_thread2()
{
    qDebug() << "调用成功";
}

//

//数据包抓取线程的函数
void startprocessing::packet_grab_thread()
{
    emit start_process_message(); //这里的内容，每次点击“开始处理”（点击停止处理时不会）运行一次
    //从初始文件中读出设定的网卡名称和采集方式
    //放在第一次运行的判断之外，防止“暂停处理”的时候更换网卡名称或采集方式
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString name_all = ini->value("/NICname_choosen/nicname").toString(); //从初始文件中获取的网卡名称
    QStringList list = name_all.split("}"); //由于初始文件中保存的网卡名称包含了网卡名称和网卡描述两个部分，因此需要分开，并将网卡名称部分提取出来
    QString nicname = list.at(0)+"}"; //以}为分界点，划分时}符号归为后一部分
    QByteArray name_bytearray = nicname.toLatin1(); //先将字符串类型转换为二进制流的形式，不可一次性转为char，转换不成功
    name = name_bytearray.data(); //将网卡名称转为char类型，便于下面使用
    int Id = ini->value("/collection_mode/Id").toInt(); //从初始文件中获取的采集方式编号
    x = ini->value("/radar_information/samplestep").toFloat()/1.25;
    min = ini->value("/radar_information/startpoint").toInt();
    max = ini->value("/radar_information/endpoint").toInt();
    num = (max - min)*4/(5.0*x); //通过观测范围和采样间距计算出采样点数
    c_time = ini->value("/collection_mode/collection_times").toInt();
    t_interval = ini->value("/collection_mode/cycle").toInt();
    s_interval = ini->value("/collection_mode/storage_interval").toInt();
    pacJudge = floor((num*2-1)/960.0); //标志位最大值,用于判断一根线分为了几个数据包
    pos = 0; //用于计数，记录当前数据包是一根数据线上的第几个数据包

    //计时器
    t_timer = new QTimer(this);
    //qDebug() << "startprocessing中的num的值是" << num;
    while(global::start_running_thread)
    {
        //Id为0代表选择了手动采集方式
        if(Id == 0)
        {
            net_ini();
            datagrab();
            //emit start_packet_data_thread(); //发送抓包数据的信号
        }

        //Id为1，平均每小时采集
        if(Id == 1)
        {
            int timeout = (60/c_time) * 1000 * 60;
            net_ini();
            connect(t_timer,SIGNAL(timeout()),this,SLOT(datagrab()));
            //t_timer->start(timeout);
            //emit start_packet_data_thread(); //发送抓包数据的信号
        }

        //Id为2,累计平均采集
        if(Id == 2)
        {
            int timeout = (60/t_interval) * 1000 * 60;
            net_ini();
            connect(t_timer,SIGNAL(timeout()),this,SLOT(datagrab()));
            t_timer->start(timeout);
            //emit start_packet_data_thread(); //发送抓包数据的信号
        }


    }
}

//过滤器设置函数
void startprocessing::net_ini()
{
    //打开适配器
    //当打开不成功时adhandle为空
    if((adhandle=pcap_open_live(name,65536,1,1000,errbuf))==nullptr) //第三个值为1即为设置成混杂模式（详情见winpcap文档）
    {
        qDebug() << "无法成功打开适配器";
        pcap_freealldevs(alldevs);
        return;
    }

    //设置网卡掩码
    netmask = 0xffffff;
    //"udp port 4377"
    //将过滤器设定为端口号
    //过滤器规则中加上port 4377后速度就下降很多
    //if(pcap_compile(adhandle,&fcode,"(ip and udp) and (src host 192.168.1.69) and(length==477)",1,netmask) < 0) //877
    /////////////////////////将过滤器条件改为采集器主机地址///////////////////////////////
    //length==1292
    //if(pcap_compile(adhandle,&fcode,"(ether src aa:bb:cc:dd:ee:ff)",1,netmask) < 0) //过滤条件为发送端的物理地址为aa:bb:cc:dd:ee:ff
    if(pcap_compile(adhandle,&fcode,"(ether src aa:bb:cc:dd:ee:ff)",1,netmask) < 0)
    {
        qDebug() << "过滤器配置失败";
        return;
    }
    //将过滤器与捕获相连，用于过滤器的设置
    if(pcap_setfilter(adhandle,&fcode) < 0)
    {
        qDebug() << "过滤器设置失败，无法用于捕获";
        pcap_freealldevs(alldevs);
        return;
    }
}

//数据包的抓取与处理函数
void startprocessing::datagrab()
{
    //捕获数据包
    //此函数用于检索下一个可用的数据包
    //当检索成立且全局变量成立时一直捕获
    //QTime time;
    //time.start();
    //qDebug() << "startprocessing_1";
    //qDebug() << "用于判断一根数据线分了几个包" << pacJudge;
    while(((res = pcap_next_ex(adhandle,&header,&pkt_data)) >= 0) && global::start_running_thread) //pcap_next_ex抓包函数
    {
        if(res == 0) //当获取报文超时时继续运行
            continue;
        //读取数据包中的计数值，用于判断当前抓取的数据包是否与上一个数据包相连
        data_point = (data *)(pkt_data); //数据包内存已经存储至结构体中
        /*pacCount = data_point->data_byte_data[16] & 0x000000ff;
        pacCount |= ((data_point->data_byte_data[17]<<8) & 0x0000ff00);
        pacCount |= ((data_point->data_byte_data[18]<<16) & 0x00ff0000);
        pacCount |= ((data_point->data_byte_data[19]<<24) & 0xff000000);*/
        //采集器角度
        angle_a = int((char*)data_point->data_byte_data[6])+int((char*)data_point->data_byte_data[7])*256;
        angle_b = data_point->data_byte_data[8] & 0x000000ff;
        angle_b |= ((data_point->data_byte_data[9]<<8) & 0x0000ff00);
        angle_b |= ((data_point->data_byte_data[10]<<16) & 0x00ff0000);
        angle_b |= ((data_point->data_byte_data[11]<<24) & 0xff000000);
        angle_c = data_point->data_byte_data[12] & 0x000000ff;
        angle_c |= ((data_point->data_byte_data[13]<<8) & 0x0000ff00);
        angle_c |= ((data_point->data_byte_data[14]<<16) & 0x00ff0000);
        angle_c |= ((data_point->data_byte_data[15]<<24) & 0xff000000);
        angle = angle_a + float(angle_c)/angle_b;
        //qDebug() << "startprocessing中的angle值和angle_old值" << angle << angle_old;
        if(count == 0)//count为组数（10个角度）
        {
            //锁采集器和paintonline之间
            if(isunLock)
            {
                lock_write->lockForWrite();
                isunLock = false;
            }

            qDebug() << "startprocessing上锁";
            //qDebug() << "startprocessing中的angle值和包数值00" << angle << angle_old;
            if(!first_run)
            {
                count++;
                //qDebug() << "startprocessing中的angle值和包数值000" << angle << angle_temp << angle_old;
                if(pacJudge == 0) //每一根数据线只有一个数据包
                {
                    angleArray[count-1] = angle_temp;
                    memcpy(&buf_write[0],&temp,(num*2+40));//有效数据头

                }
                else
                {
                      //qDebug() << "startprocessing中的angle值和包数值00000" << angle << angle_temp << angle_old;
                    angleArray[count-1] = angle_temp;
                      //qDebug() << "startprocessing中的angle值和包数值0000" << angle << angle_temp << angle_old;
                    memcpy(&buf_write[0],&temp,1000);

                }
                //qDebug() << "startprocessing中的angle值和包数值0" << angle << angle_old;
            }
            else
            {
                //qDebug() << "startprocessing中的angle值和包数值1" << angle << angle_old;
                if(data_point->data_byte_data[4] != 0) //（unchar一个字节数）当抓取到的数据包不是一根数据线的第一个包时，重新循环抓取下一个数据包
                    continue;
                //pacCountOld = pacCount - 1;
                //qDebug() << "startprocessing_2";
                angle_old = angle;
                first_run = false;
            }
        }
        //当前数据包与上一个数据包不相连
        /*if(pacCount != pacCountOld + 1)
        {
            if(data_point->data_byte_data[4] != 0) //当抓取到的数据包不是一根数据线的第一个包时，重新循环抓取下一个数据包
                continue;
            angle_old = angle;
        }
        pacCountOld = pacCount;*/

        //qDebug() << "startprocessing中的angle值和包数值2" << angle << angle_old;
        //角度差值，预防漏包（跨0）
        if(angle < angle_old)
        {
            angleCha = 360 - angle_old + angle;
        }
        else
        {
            angleCha = angle - angle_old;
        }
        //qDebug() << "startprocessing_3" << angle << angle_old << angleCha;
        if(angleCha < N)
        {
            //qDebug() << "startprocessing_4" << pacJudge << data_point->data_byte_data[4];
            if(pacJudge == 0 && data_point->data_byte_data[4] == 0) //每一根数据线只有一个数据包
            {
                count++;
                //qDebug() << "只有一个数据包" << count;
                angleArray[count-1] = angle;
                memcpy(&buf_write[(count-1)*(num*2+40)],&data_point->data_byte_data,(num*2+40));
            }
            else
            {
                //qDebug() << "不止一个数据包" << count;
                if(pacJudge != 0 && data_point->data_byte_data[4] == 0) //当标志位为0时,此数据包为一根数据线的第一个包，一定有960字节的数据
                {
                    count++;
                    angleArray[count-1] = angle;
                    memcpy(&buf_write[(count-1)*(num*2+40)],&data_point->data_byte_data,1000);
                }
                else if(pacJudge != 0 && data_point->data_byte_data[4] != pacJudge) //当数据包不是一根数据线的第一个包，也不是最后一个数据包时
                {
                    count++;
                    pos++;
                    angleArray[count-1] = angle;
                    memcpy(&buf_write[(count-1)*(num*2+40)+pos*960+40],&data_point->data_byte_data[40],960);
                }
                else if(pacJudge != 0 && data_point->data_byte_data[4] == pacJudge) //当标志位为1但一根数据线分为了多个包时
                {
                    count++;
                    angleArray[count-1] = angle;
                    memcpy(&buf_write[(count-1)*(num*2+40)+1000+pos*960],&data_point->data_byte_data[40],num*2-960*(pos+1));
                }
            }
        }
        else //数据包已抓满，进入显示，开锁（锁的位置需要调试，以防上锁情况）
        {
            //qDebug() << "startprocessing_5";
            //当变量为真时，说明其余线程已完成了对共享内存的读取工作
            qDebug() << "startprocessing中传给paintonline的count值为" << count;
            emit preprocessing(count);
            lock_write->unlock();

            qDebug() << "startprocessing中发送信号Preprocessing";
            isunLock = true;
            count = 0; //当计数达到100时，计数清零
            angle_old = angle;
            if(pacJudge == 0) //每一根数据线只有一个数据包
            {
                angle_temp = angle;
                memcpy(&temp,&data_point->data_byte_data,(num*2+40));
            }
            else
            {
                angle_temp = angle;
                memcpy(&temp,&data_point->data_byte_data,1000);
            }
        }
    }
}
startprocessing::~startprocessing()
{
    delete []last_end;
    //delete []buf_cap;
}
