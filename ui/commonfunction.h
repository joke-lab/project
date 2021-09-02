#ifndef COMMONFUNCTION_H
#define COMMONFUNCTION_H
#include <QtGlobal>
#include <QSettings>
#include <QCoreApplication>
#include <QApplication>
#include <math.h>
#include <QObject>

class commonfunction
{
public:
    commonfunction();
    ~commonfunction();
    void CoCIRFun(int angle, int point, qint16 *buffer);
    int fGateStep;
    int nGrade;
    float fGate; //抑制门限
    //反异步干扰处理的中间变量数组
    qint16 *tempData;
    qint16 *tempData1;
    qint16 *tempData2;
    qint16 *tempEnd; //保留数组最后一根线
    qint16 *buffer;
};

#endif // COMMONFUNCTION_H
