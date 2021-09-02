#include "color_set.h"
#include "ui_color_set.h"

/*
//color_set类是“颜色设置”界面的类，包含了界面的控件布局和功能的实现。
//界面的左边是色度条和可观测范围值条，界面的右边从上至下是颜色显示框和选择颜色按钮、最小可观测值和最大可观测值的微调框、保存和取消按钮
//打开界面时，色度条的颜色、最小和最大可观测值的初始值都是从ini文件中读取到的。
//色度条由13个标签组成，每个标签都带有鼠标事件，点击时，在右边的颜色框中会显示对应的颜色，点击选择颜色按钮，会出现调色盘界面，
//在调色盘中选择颜色后，点击确认后调色盘界面关闭，选择的颜色显示在颜色框和对应的标签中，以此达到改变颜色条中颜色的目的。
//在微调框中设置最小和最大可观测强度值，可通过手动输入和点击上下按钮改变值，值改变后，左边的可观测范围值条将根据改变后的值实时进行变化，
//点击保存按钮后，将当前界面的色度条的每个颜色值、最小和最大可观测值都存入ini文件中，以便下次打开界面时调用；此界面关闭。
//点击退出按钮后，此界面直接关闭，操作将不被记录。
*/

color_set::color_set(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::color_set)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    //新建两个横向布局，分别放入两个widget中
    //后续所有显示颜色和强度值的label都放入这两个横向布局中
    QVBoxLayout *part1_colorbar_vLayout = new QVBoxLayout;
    QVBoxLayout *part1_strength_vLayout = new QVBoxLayout;
    ui->part1_colorbar->setLayout(part1_colorbar_vLayout);
    ui->part1_strength->setLayout(part1_strength_vLayout);

    //调用函数，添加颜色条和强度值
    colorBar(part1_colorbar_vLayout,part1_strength_vLayout);

    //从ini文件中读取最大可观测值，用于spinbox初始显示
    QSettings *spinbox_Value = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int spinbox = spinbox_Value->value("/color_strength/a").toInt(); //从初始文件中读取出最大可观测强度值
    int mini_spinbox = spinbox_Value->value("/color_strength/b").toInt(); //从初始文件中读取出最小可观测强度值
    ui->strength_spin->setValue(spinbox); //显示出初始的最大可观测值
    ui->mini_strength_spin->setValue(mini_spinbox); //显示出初始的最小可观测值

    color_edit->setStyleSheet("background-color:rgba(0,0,0)"); //为color_edit设置初始颜色值为白色
    color_edit->setFocusPolicy(Qt::NoFocus); //设置color_edit无法编辑，只能显示颜色


    //为13个颜色label设置事件过滤器
    for(int i=0;i<16;i++)
    {
        (color_label+i)->installEventFilter(this);
    }

    //关闭两个微调框的键盘跟踪功能
    ui->strength_spin->setKeyboardTracking(false);
    ui->mini_strength_spin->setKeyboardTracking(false);

    QObject::connect(ui->initial,SIGNAL(clicked(bool)),this,SLOT(initial_color_slot()));
    QObject::connect(ui->initial2,SIGNAL(clicked(bool)),this,SLOT(initial2_color_slot()));
    QObject::connect(ui->initial3,SIGNAL(clicked(bool)),this,SLOT(initial3_color_slot()));
    QObject::connect(ui->initial4,SIGNAL(clicked(bool)),this,SLOT(initial4_color_slot()));
    QObject::connect(ui->chooseColor,SIGNAL(clicked(bool)),this,SLOT(colorDialog_slot())); //调色板按钮的信号与槽的链接
    QObject::connect(ui->confirm,SIGNAL(clicked(bool)),this,SLOT(confirm_slot())); //确认按钮的信号与槽的链接
    QObject::connect(ui->strength_spin,QOverload<int>::of(&QSpinBox::valueChanged), //当最大可观测强度值的微调框的值改变时调用的函数
                     [=](int value)
    {
        int number1_int = value;
        int number2_int = ui->mini_strength_spin->value();
        QFont number_Font("Microsoft YaHei",10,75); //设置一种字体格式，括号内分别为字体、大小和权重
        //强度值部分，设置17个label显示对应的强度值
        for(int i=0;i<17;i++)
        {
            if(i==0)
            {
                //最大可观测强度值的显示
                strength_label[0].setText(QString::number(number1_int));
                strength_label[0].setFont(number_Font);
            }
            else if(i==16)
            {
                //最小可观测强度值的显示
                strength_label[16].setText(QString::number(number2_int));
                strength_label[16].setFont(number_Font);
            }
            else
            {
                //其余强度值的显示
                strength_label[i].setText(QString::number(int(double((number1_int-number2_int)/16.0)*double(16-i))+number2_int));
                strength_label[i].setFont(number_Font);
            }
        }
    });
    QObject::connect(ui->mini_strength_spin,QOverload<int>::of(&QSpinBox::valueChanged), //当最小可观测强度值的微调框的值改变时调用的函数
                     [=](int value)
    {
        int number1_int = ui->strength_spin->value();
        int number2_int = value;
        QFont number_Font("Microsoft YaHei",10,75); //设置一种字体格式，括号内分别为字体、大小和权重
        //强度值部分，设置13个label显示对应的强度值
        for(int i=0;i<17;i++)
        {
            if(i==0)
            {
                //最大可观测强度值的显示
                strength_label[0].setText(QString::number(number1_int));
                strength_label[0].setFont(number_Font);
            }
            else if(i==16)
            {
                //最小可观测强度值的显示
                strength_label[16].setText(QString::number(number2_int));
                strength_label[16].setFont(number_Font);
            }
            else
            {
                //其余强度值的显示
                strength_label[i].setText(QString::number(int(double((number1_int-number2_int)/16.0)*double(16-i))+number2_int));
                strength_label[i].setFont(number_Font);
            }
        }
    });
}

//事件过滤器函数
//用于所有的颜色label接收鼠标信号
bool color_set::eventFilter(QObject *watched, QEvent *event)
{
    for(int i=0;i<16;i++)
    {
        if(watched == (color_label+i) && event->type() == QEvent::MouseButtonPress)
        {
            i_change = i; //将当前选中的颜色框的位置赋给i_change，以便后面改变其颜色时使用
            eventfilter_palette = (color_label+i)->palette(); //获取color1当前的颜色状态
            //实现了将颜色label中的颜色设置到color_edit中
            eventfilter_red = QString::number(eventfilter_palette.window().color().red());
            eventfilter_green = QString::number(eventfilter_palette.window().color().green());
            eventfilter_blue = QString::number(eventfilter_palette.window().color().blue());
            //将color1的颜色状态给color_edit
            ui->color_edit->setStyleSheet("background-color:rgb("+eventfilter_red+","+eventfilter_green+","+eventfilter_blue+")");
            return true;
        }
    }
    return false;
}

//色度条和强度值部分
void color_set::colorBar(QVBoxLayout *part1_colorbar_vLayout,QVBoxLayout *part1_strength_vLayout)
{
    //新建first和second布局，分别对应颜色条和强度值
    QVBoxLayout *first = new QVBoxLayout;
    QVBoxLayout *second = new QVBoxLayout;
    first = part1_colorbar_vLayout;
    second = part1_strength_vLayout;
    first->setSpacing(0); //设置间距为0
    second->setMargin(0); //设置边缘距离为0

    //打开ini文件
    QSettings *color_ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    //读取出最大可观测强度值
    QString number1 = color_ini->value("/color_strength/a").toString(); //从初始文件中读取出最大可观测强度
    QString number2 = color_ini->value("/color_strength/b").toString(); //从初始文件中读取出最小可观测强度
    int number1_int = number1.toInt(); //将最大值转为int类型，在后面使用最小观测值和最大观测值的差求其余点的值时使用
    int number2_int = number2.toInt(); //将最小值转为int类型，在后面使用最小观测值和最大观测值的差求其余点的值时使用
    QFont number_Font("Microsoft YaHei",10,75); //设置一种字体格式，括号内分别为字体、大小和权重

    //label定义为全局私有变量，设置12个label显示颜色
    for(int i =0;i<16;i++)
    {
        QString b('a'+i); //实现小写字母从a到m的循环
        (color_label+i)->setAutoFillBackground(true); //自动填满，否则无法实现
        (color_label+i)->setPalette(color_ini->value("/color/"+b).value<QPalette>()); //从ini文件中读取出qpalette的值赋给色度条
        first->addWidget(color_label+i);
    }

    //强度值部分，设置13个label显示对应的强度值
    for(int i=0;i<17;i++)
    {
        if(i==0)
        {
            strength_label[0].setText(number1);
            strength_label[0].setFont(number_Font);
            second->addWidget(&strength_label[0]);
        }
        else if(i==16)
        {
            strength_label[16].setText(number2);
            strength_label[16].setFont(number_Font);
            second->addWidget(&strength_label[16]);
        }
        else
        {
            strength_label[i].setText(QString::number(int(double((number1_int-number2_int)/16.0)*double(16-i))+number2_int));
            strength_label[i].setFont(number_Font);
            second->addWidget(&strength_label[i]);
        }
    }
}

//调色盘按钮的槽函数,打开调色盘
void color_set::colorDialog_slot()
{
    colordialog = new QColorDialog; //调色盘的实例化
    //当在调色盘中选中一种颜色时，发送信号函数colorSelected(自带)，调用函数获取选中的颜色
    QObject::connect(colordialog,SIGNAL(colorSelected(QColor)),this,SLOT(getcolorSlot(QColor)));
    colordialog->exec(); //运行调色盘
}

//当调色盘点击ok键时，获取其选中的颜色的槽函数
void color_set::getcolorSlot(QColor cor)
{
    //获取当前调色盘选中颜色的rgb值
    QString colorStr = QString("background-color:rgb(%1,%2,%3);").arg(cor.red()).arg(cor.green()).arg(cor.blue());
    ui->color_edit->setStyleSheet(colorStr); //将编辑栏的颜色设置为调色盘选中的颜色
    (color_label+i_change)->setStyleSheet(colorStr); //将当前鼠标选中的颜色块变为调色盘选中的颜色
}

//颜色初始化按钮的槽函数
//当点击此按钮时，将色度条上的颜色回归初始颜色
void color_set::initial_color_slot()
{
    int R_int;
    int G_int;
    int B_int;
    //label定义为全局私有变量，设置12个label显示颜色
    for(int i =0;i<16;i++)
    {
        QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        QString b('a'+i); //实现小写字母从a到m的循环
        R_int = ini->value(("/color/"+b+"R").toLatin1()).toInt(); //将初始文件中的第一个颜色的R通道进行读取
        G_int = ini->value(("/color/"+b+"G").toLatin1()).toInt(); //将初始文件中的第一个颜色的G通道进行读取
        B_int = ini->value(("/color/"+b+"B").toLatin1()).toInt(); //将初始文件中的第一个颜色的B通道进行读取
        //将rgb值赋给每个颜色块
        (color_label+i)->setStyleSheet(QString("background-color:rgb(%1,%2,%3);").arg(R_int).arg(G_int).arg(B_int));
    }
}

//模板2按钮的槽函数
//当点击此按钮时，将色度条上的颜色回归第二种渐变的初始颜色
void color_set::initial2_color_slot()
{
    int R_int;
    int G_int;
    int B_int;
    //label定义为全局私有变量，设置12个label显示颜色
    for(int i =0;i<16;i++)
    {
        QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        QString b('a'+i); //实现小写字母从a到m的循环
        R_int = ini->value(("/color2/"+b+"R").toLatin1()).toInt(); //将初始文件中的第一个颜色的R通道进行读取
        G_int = ini->value(("/color2/"+b+"G").toLatin1()).toInt(); //将初始文件中的第一个颜色的G通道进行读取
        B_int = ini->value(("/color2/"+b+"B").toLatin1()).toInt(); //将初始文件中的第一个颜色的B通道进行读取
        //将rgb值赋给每个颜色块
        (color_label+i)->setStyleSheet(QString("background-color:rgb(%1,%2,%3);").arg(R_int).arg(G_int).arg(B_int));
    }
}

//模板3按钮的槽函数
//当点击此按钮时，将色度条上的颜色回归第二种渐变的初始颜色
void color_set::initial3_color_slot()
{
    int R_int;
    int G_int;
    int B_int;
    //label定义为全局私有变量，设置12个label显示颜色
    for(int i =0;i<16;i++)
    {
        QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        QString b('a'+i); //实现小写字母从a到m的循环
        R_int = ini->value(("/color3/"+b+"R").toLatin1()).toInt(); //将初始文件中的第一个颜色的R通道进行读取
        G_int = ini->value(("/color3/"+b+"G").toLatin1()).toInt(); //将初始文件中的第一个颜色的G通道进行读取
        B_int = ini->value(("/color3/"+b+"B").toLatin1()).toInt(); //将初始文件中的第一个颜色的B通道进行读取
        //将rgb值赋给每个颜色块
        (color_label+i)->setStyleSheet(QString("background-color:rgb(%1,%2,%3);").arg(R_int).arg(G_int).arg(B_int));
    }
}

//模板4按钮的槽函数
//当点击此按钮时，将色度条上的颜色回归第二种渐变的初始颜色
void color_set::initial4_color_slot()
{
    int R_int;
    int G_int;
    int B_int;
    //label定义为全局私有变量，设置12个label显示颜色
    for(int i =0;i<16;i++)
    {
        QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        QString b('a'+i); //实现小写字母从a到m的循环
        R_int = ini->value(("/color4/"+b+"R").toLatin1()).toInt(); //将初始文件中的第一个颜色的R通道进行读取
        G_int = ini->value(("/color4/"+b+"G").toLatin1()).toInt(); //将初始文件中的第一个颜色的G通道进行读取
        B_int = ini->value(("/color4/"+b+"B").toLatin1()).toInt(); //将初始文件中的第一个颜色的B通道进行读取
        //将rgb值赋给每个颜色块
        (color_label+i)->setStyleSheet(QString("background-color:rgb(%1,%2,%3);").arg(R_int).arg(G_int).arg(B_int));
    }
}
//确认按钮的槽函数
void color_set::confirm_slot()
{
    //当点击确认时，将此时设置的最大观测强度值、最小观测距离、最大观测距离存入ini文件中
    QSettings *color_ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    color_ini->setValue("/color_strength/a",QString::number(ui->strength_spin->value())); //将最大观测强度值存入初始文件中
    color_ini->setValue("/color_strength/b",QString::number(ui->mini_strength_spin->value())); //将最小观测强度值存入初始文件中

    //label定义为全局私有变量，设置12个label显示颜色
    for(int i =0;i<16;i++)
    {
        QString b('a'+i); //实现小写字母从a到m的循环
        color_ini->setValue("/color/"+b,(color_label+i)->palette()); //将当前色度条上的qpalette存入ini文件
    }
    emit colorbar_change(); //发送信号回主界面改变色度条
    //将此全局变量变为正
    //即当再进行显示的时候，会重新读取ini文件中的观测强度范围和颜色值
    global::run_firsttime = true;
    this->close();
}

//取消按钮的槽函数
void color_set::cancel_slot()
{
    this->close();
}

color_set::~color_set()
{
    delete ui;
    for(int i=0;i<16;i++)
    {
        delete (color_label + i);
    }
}

