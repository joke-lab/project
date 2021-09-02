#include "commonfunction.h"

commonfunction::commonfunction()
{

}

commonfunction::~commonfunction()
{
}

//反异步干扰函数
void commonfunction::CoCIRFun(int angle, int point, qint16 *buffer)
{
    if(angle < 3) //线数必须大于3
        return;
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    //bool state = ini->value("/radar_information/state").toBool(); //判断是否需要反异步干扰的状态
    int fGateStep = ini->value("/radar_information/gatestep").toInt();
    int nGrade = ini->value("/radar_information/grade").toInt();
    float fGate = fGateStep * nGrade;
    //bool first_time = true; //数组第一次进入函数时为正
    tempData = new qint16[point]; //存放反异步干扰数据的数组
    tempData1 = new qint16[point];
    tempData2 = new qint16[point];
    buffer = new qint16[angle*point];
    if(angle == 3600)
    {
        for(int i = 0; i < angle; i++)
        {
            int nPre = i - 1;
            int nData = i;
            int nAft = i + 1;
            if(nPre < 0)
            {
                nPre = angle-1;
            }

            if(nAft >= angle)
            {
                nAft = 0;
            }

            memcpy(&tempData1[0],&buffer[nPre*point],point*sizeof(qint16)); //第一根线的数据复制
            memcpy(&tempData[0],&buffer[nData*point],point*sizeof(qint16)); //第二根线的数据复制,(基准线)
            memcpy(&tempData2[0],&buffer[nAft*point],point*sizeof(qint16)); //第三根线的数据复制

            for(int j = 0; j < point; j++)
            {
                float fGateTemp1 = fabs(tempData1[j] - tempData[j]);
                float fGateTemp2 = fabs(tempData2[j] - tempData[j]);

                if(fGateTemp1 > fGate && fGateTemp2 > fGate)
                    tempData[j] = (tempData1[j] + tempData2[j])/2;
            }
            memcpy(&buffer[i*point],&tempData[0],point*sizeof(qint16));
        }
    }
    else
    {
        for(int i = 0; i < angle; i++)
        {
            int nPre = i - 1;
            int nData = i;
            int nAft = i + 1;
            if(nPre < 0)
            {
                nPre = i+1;
                nData = i;
                nAft = i+2;
            }

            if(nAft > angle)
            {
                nAft = i-1;
                nData = i;
                nPre = i-2;
            }

            memcpy(&tempData1[0],&buffer[nPre*point],point*sizeof(qint16)); //第一根线的数据复制
            memcpy(&tempData[0],&buffer[nData*point],point*sizeof(qint16)); //第二根线的数据复制,(基准线)
            memcpy(&tempData2[0],&buffer[nAft*point],point*sizeof(qint16)); //第三根线的数据复制

            for(int j = 0; j < point; j++)
            {
                float fGateTemp1 = fabs(tempData1[j] - tempData[j]);
                float fGateTemp2 = fabs(tempData2[j] - tempData[j]);

                if(fGateTemp1 > fGate && fGateTemp2 > fGate)
                    tempData[j] = (tempData1[j] + tempData2[j])/2;
            }
            memcpy(&buffer[i*point],&tempData[0],point*sizeof(qint16));
        }
        /*tempEnd = new qint16[point];
        //qDebug()<<"反异步干扰"<<fGate;
        for(int i = 0; i < angle; i++)
        {
            int nPre = i - 1;
            int nData = i;
            int nAft = i + 1;
            if(first_time)
            {
                first_time = false;
                if(nPre < 0)
                {
                    nPre = 1;
                    nData = 0;
                    nAft = 2;
                }
            }

            memcpy(&tempData1[0],&buffer[nPre*point],point*sizeof(qint16)); //第一根线的数据复制
            memcpy(&tempData[0],&buffer[nData*point],point*sizeof(qint16)); //第二根线的数据复制,(基准线)
            memcpy(&tempData2[0],&buffer[nAft*point],point*sizeof(qint16)); //第三根线的数据复制

            if(nAft > angle)
            {
                memcpy(&tempEnd[0],&buffer[nData*point],point*sizeof(qint16)); //保留数组的最后一根线数据，为之后的计算显示做准备
            }

            if(!first_time)
            {
                if(nPre < 0)
                {
                    nPre = 0;
                    nData = 0;
                    nAft = 1;
                }
                memcpy(&tempData1[0],&tempEnd[0],point*sizeof (qint16));
            }

            for(int j = 0; j < point; j++)
            {
                float fGateTemp1 = fabs(tempData1[j] - tempData[j]);
                float fGateTemp2 = fabs(tempData2[j] - tempData[j]);

                if(fGateTemp1 > fGate && fGateTemp2 > fGate)
                    tempData[j] = (tempData1[j] + tempData2[j])/2;
            }
            memcpy(&buffer[i*point],&tempData[0],point*sizeof(qint16));
        }*/
    }
    delete [] tempEnd;
    delete [] tempData;
    delete [] tempData1;
    delete [] tempData2;
}
