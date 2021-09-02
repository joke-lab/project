#include "batchproc.h"
#include "ui_batchproc.h"

batchproc::batchproc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::batchproc)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    //显示上次输出.xlsx和.txt的存储路径
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int check = ini->value("/batchprocess/xlsx").toInt();
    QString path = ini->value("/batchprocess/xlsxPath").toString();
    QString file = ini->value("/batchprocess/xlsxName").toString();
    ui->check_xlsx->setChecked(check);
    ui->excelPath->setText(path);
    ui->excelName->setText(file);
    if(!check)
    {
        ui->excelPath->setEnabled(false);
        ui->excelP->setEnabled(false);
        ui->excelName->setEnabled(false);
    }
    check = ini->value("/batchprocess/txt").toInt();
    path = ini->value("/batchprocess/txtPath").toString();
    ui->check_txt->setChecked(check);
    ui->txtPath->setText(path);
    if(!check)
    {
        ui->txtPath->setEnabled(false);
        ui->txtP->setEnabled(false);
    }

    //新开一个子线程用于处理数据
    thread = new QThread;
    files_deal->moveToThread(thread);
    connect(thread,&QThread::finished,files_deal,&QObject::deleteLater);
    connect(thread,&QThread::finished,thread,&QObject::deleteLater);
    thread->start();
    QObject::connect(ui->add,SIGNAL(clicked(bool)),this,SLOT(addfile_Slot())); //添加
    QObject::connect(ui->delall,SIGNAL(clicked(bool)),this,SLOT(delall_Slot())); //全部清除
    QObject::connect(ui->del,SIGNAL(clicked(bool)),this,SLOT(delefile_Slot())); //删除
    QObject::connect(ui->areaChoice,SIGNAL(clicked(bool)),this,SLOT(areachoice_Slot())); //区域选择
    QObject::connect(ui->sysSet,SIGNAL(clicked(bool)),this,SLOT(sysset_Slot())); //系统设置
    QObject::connect(ui->deal,SIGNAL(clicked(bool)),this,SLOT(deal_Slot())); //开始处理
    QObject::connect(ui->close,SIGNAL(clicked(bool)),this,SLOT(close_Slot())); //关闭
    QObject::connect(ui->exPort,SIGNAL(clicked(bool)),this,SLOT(export_Slot())); //导出
    //批处理模块中主子线程信号与槽的连接
    QObject::connect(this,SIGNAL(files_initial()),files_deal,SLOT(filesInitial_Slot()));
    QObject::connect(this,SIGNAL(files_lkFigure(int,QString,QString)),files_deal,SLOT(lkDeal_Slot(int,QString,QString)));
    QObject::connect(this,SIGNAL(files_ptFigure(int,QString,QString)),files_deal,SLOT(ptDeal_Slot(int,QString,QString)));
    QObject::connect(this,SIGNAL(files_rrawFigure(int,QString,QString,QSqlQuery *)),files_deal,SLOT(rrawDeal_Slot(int,QString,QString,QSqlQuery *)));
    QObject::connect(files_deal,SIGNAL(filesResult(int,double,double,double *,QString)),this,SLOT(filesResult_Slot(int,double,double,double *,QString)));
    QObject::connect(ui->check_txt,SIGNAL(clicked(bool)),this,SLOT(outFileType_Slot()));
    QObject::connect(ui->check_xlsx,SIGNAL(clicked(bool)),this,SLOT(outFileType_Slot()));
    QObject::connect(ui->txtP,SIGNAL(clicked(bool)),this,SLOT(chooseTxtPath())); //选择输出路径
    QObject::connect(ui->excelP,SIGNAL(clicked(bool)),this,SLOT(chooseExcelPath())); //选择输出路径

}

//输出文件格式部分的界面功能槽函数
void batchproc::outFileType_Slot()
{
    if(ui->check_txt->isChecked())
    {
        ui->txtPath->setEnabled(true);
        ui->txtP->setEnabled(true);
    }
    else
    {
        ui->txtPath->setEnabled(false);
        ui->txtP->setEnabled(false);
    }
    if(ui->check_xlsx->isChecked())
    {
        ui->excelPath->setEnabled(true);
        ui->excelP->setEnabled(true);
        ui->excelName->setEnabled(true);
    }
    else
    {
        ui->excelPath->setEnabled(false);
        ui->excelP->setEnabled(false);
        ui->excelName->setEnabled(false);
    }
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int check = ui->check_xlsx->isChecked();
    ini->setValue("/batchprocess/xlsx",check);
    ini->setValue("/batchprocess/xlsxPath",ui->excelPath->text());
    check = ui->check_txt->isChecked();
    ini->setValue("/batchprocess/txt",check);
    ini->setValue("/batchprocess/txtPath",ui->txtPath->text());
}

//选择输出txt文件路径的槽函数
void batchproc::chooseTxtPath()
{
    QString directory = ui->txtPath->text(); //读取当前数据文件编辑栏中的文件绝对路径
    qDebug() << "directory" << directory;
    if(directory.isEmpty()) //若当前编辑栏中为空
    {
        QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("save path"),QDir::currentPath()));
        if(!directory.isEmpty())
        {
            ui->txtPath->setText(directory);
        }
        qDebug() << "directory1" << directory;
    }
    else
    {
        QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("save path"),directory));
        if(!directory.isEmpty())
        {
            ui->txtPath->setText(directory);
        }
        qDebug() << "directory2" << directory;
    }
}

//选择输出excel文件路径的槽函数
void batchproc::chooseExcelPath()
{
    QString directory = ui->excelPath->text(); //读取当前数据文件编辑栏中的文件绝对路径
    if(directory.isEmpty()) //若当前编辑栏中为空
    {
        QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("save path"),QDir::currentPath()));
        if(!directory.isEmpty())
        {
            ui->excelPath->setText(directory);
        }
    }
    else
    {
        QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("save path"),directory));
        if(!directory.isEmpty())
        {
            ui->excelPath->setText(directory);
        }
    }
}


//添加文件按钮的槽函数
void batchproc::addfile_Slot()
{
    //从初始文件中获取默认打开路径
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    dirr = ini->value("/batchprocess_path/path").toString();

    if(dirr == nullptr)
    {
        files = QFileDialog::getOpenFileNames(this,"选择一个或多个文件打开",QDir::currentPath());
        //filename = QFileDialog::getOpenFileName(this,"打开",QDir::currentPath()); //将执行文件所在路径设置为打开的路径
    }
    else
    {
        files = QFileDialog::getOpenFileNames(this,"选择一个或多个文件打开",dirr);
        //filename = QFileDialog::getOpenFileName(this,"打开",dirr);
    }
    if(files.count() != 0)
    {
        for(int i = 0;i<files.count(); i++)
        {
            fi = QFileInfo(files.at(i));
            row = ui->tab->rowCount(); //获取当前的行数
            ui->tab->insertRow(row); //在当前行数的基础上增加一行
            ui->tab->setItem(row, 0, new QTableWidgetItem(QString(fi.fileName() + " " + fi.absolutePath()))); //在第一列显示文件名称
            //tab->setItem(row, 1, new QTableWidgetItem(fi.absolutePath())); //在第二列显示文件的绝对路径
            qDebug() << fi.fileName();
        }
        ini->setValue("/batchprocess_path/path",files.at(0)); //将当前打开的路径存储为默认路径
    }
    //tab->sortItems(0,Qt::AscendingOrder);
}

//全部清除文件按钮的槽函数
void batchproc::delall_Slot()
{
    //ui->tab->clearContents(); //删除除表头外的所有内容
    int count = ui->tab->rowCount();
    for(int i=0;i<count;i++)
        ui->tab->removeRow(0);
}

//删除选中文件按钮的槽函数
void batchproc::delefile_Slot()
{
    QList<QTableWidgetSelectionRange> sRangeList = ui->tab->selectedRanges(); //获取选中的行号列表
    if(sRangeList.length() == 0)
    {
        QMessageBox::warning(nullptr,"提示" ,"请先选中需要删除的文件所在行",QMessageBox::Ok,0);
    }
    else
    {
        for(const auto &p : qAsConst(sRangeList))
        {
            for (int i = p.topRow() + p.rowCount() - 1; i > p.topRow() - 1; i--)
            {
                ui->tab->removeRow(i);
            }
        }
    }
}

//区域选择按钮的槽函数
void batchproc::areachoice_Slot()
{
    emit open_regionselection();
}

//系统设置按钮的槽函数
void batchproc::sysset_Slot()
{
    emit open_system();
}

//开始处理按钮的槽函数
void batchproc::deal_Slot()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int check = ui->check_xlsx->isChecked();
    ini->setValue("/batchprocess/xlsx",check);
    ini->setValue("/batchprocess/xlsxPath",ui->excelPath->text());
    check = ui->check_txt->isChecked();
    ini->setValue("/batchprocess/txt",check);
    ini->setValue("/batchprocess/txtPath",ui->txtPath->text());
    ini->setValue("/batchprocess/xlsxName/",ui->excelName->text()); //excel文件名称
    emit files_initial(); //调用子线程的初始化函数
    if(ui->check_txt->isChecked())
    {
        if(ui->txtPath)
        {
            txtFilepath = ui->txtPath->text(); //输出文件的绝对路径
        }
    }
    if(ui->check_xlsx->isChecked())
    {
        if(ui->excelPath && ui->excelName)
        {
            excelFilename = ui->excelPath->text() + ui->excelName->text() +".xlsx"; //输出文件的绝对路径
        }
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbTemp");
    if (!db.open())
    {
        QMessageBox::information(nullptr,"数据库连接失败","SQLite数据库连接失败，请检查软件版本并重新启动",1,QMessageBox::Ok);
    }
    QSqlQuery *query = new QSqlQuery(db);
    for(int i=0;i<ui->tab->rowCount();i++)
    {
        //qDebug() << "进入循环" << tab->item(i,1)->text();
        //qDebug() << "进入循环" << tab->item(i,0)->text();
        QString str = ui->tab->item(i,0)->text();
        int pos1 = str.indexOf(" ");
        QString fil = str.mid(pos1+1);
        file_path = fil + "/" + str.left(pos1);
        qDebug() << "，，，" << file_path;
        //file_path = tab->item(i,1)->text() + "/" + tab->item(i,0)->text();
        //qDebug() << "，，，" << file_path;
        datatype = ui->comboBox->currentText();
        QString fileName = ui->tab->item(i,0)->text();
        int pos2 = fileName.indexOf("2");
        QString search = fileName.mid(pos2,14);
        //qDebug() << "当前的数据类型为" << datatype;
        //连接数据库
        if(datatype == "龙口数据格式")
        {
            emit files_lkFigure(i,file_path,search);
        }
        else if(datatype == "平潭数据格式")
        {
            emit files_ptFigure(i,file_path,search);
        }
        else if(datatype == "RRAW数据格式")
        {
            emit files_rrawFigure(i,file_path,search,query);
        }
    }
    query->finish();
}
//在界面上显示计算结果
void batchproc::filesResult_Slot(int i,double d6,double d7,double *d,QString d9)
{
    ui->tab->setItem(i, 1,new QTableWidgetItem(QString::number(d6,'f',3))); //经度
    ui->tab->setItem(i, 2,new QTableWidgetItem(QString::number(d7,'f',3))); //纬度
    ui->tab->setItem(i, 3, new QTableWidgetItem(QString::number(d[5],'f',4))); //有效波高
    ui->tab->setItem(i, 4, new QTableWidgetItem(QString::number(d[3],'f',4))); //峰波周期
    ui->tab->setItem(i, 5, new QTableWidgetItem(QString::number(d[2],'f',4))); //峰波频率
    ui->tab->setItem(i, 6, new QTableWidgetItem(QString::number(d[4],'f',4))); //峰波波向
    ui->tab->setItem(i, 7, new QTableWidgetItem(QString::number(d[0],'f',4))); //流速
    ui->tab->setItem(i, 8, new QTableWidgetItem(QString::number(d[1],'f',4))); //流向
    ui->tab->setItem(i, 9, new QTableWidgetItem(d9)); //数据时间
    qDebug() << "调用的i" << i;
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int check = ini->value("/batchprocess/xlsx").toInt();
    if(check)
    {
        if(i == 0)
        {
            tempData = new double[16]; //一次性存入数据时
            //tempData = new double[16]; //每行单独存入数据时
            memcpy(&tempData[0],d,16*sizeof (double));
        }
        else
        {
            memcpy(&tempData[0],d,16*sizeof (double));
        }
        excelData.clear();
        excelData.append(d9);
        QString temp = ui->tab->item(i,0)->text();
        QString save = temp.left(temp.indexOf(" "));
        excelData.append(save);
        excelData.append(d6);
        excelData.append(d7);
        for(int m=0;m<16;m++)
        {
            excelData.append(tempData[m]);
        }
        allData.append(excelData);
        qDebug() << "数组大小" << excelData.size();
        if(i == ui->tab->rowCount() - 1)
        {

            //qDebug() << "插入文件中的数据位置" << res;
            QList<QVariant>saveData;
            for(int m=0;m<ui->tab->rowCount();m++)
            {
                saveData.append(allData.at(m));
            }
            fileName = ui->excelPath->text() + "\\" + ui->excelName->text() + ".xlsx";    //设置可选择的保存的文件名xls或txt文件
            fileName.replace(QString("\\"), QString("/"));
            qDebug() << "filename" << fileName;
            if(fileName != NULL) //xls文件为空的时候创建xls文件
            {
                //file1.setFileName(excelName->text() + ".xlsx");
                QAxObject *excel = new QAxObject;
                if(excel->setControl("Excel.Application")) //连接Excel控件
                {
                    excel->dynamicCall("SetVisible (bool Visible)",false); //不显示窗体
                    excel->setProperty("DisplayAlerts",false); //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
                    QAxObject *workbooks = excel->querySubObject("WorkBooks");            //获取工作簿集合
                    workbooks->dynamicCall("Add");                                        //新建一个工作簿
                    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");        //获取当前工作簿
                    QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1); //获取打开的工作簿的第一个sheet
                    QAxObject *cell;

                    //添加Excel表头数据
                    QStringList list;
                    list << "时间" << "文件名称" << "经度" << "纬度"
                         << "流速" << "流向" << "峰波频率" << "峰波周期"
                         << "峰波波向" << "有效波高" << "信噪比" << "零阶矩阵" << "十分之一波高"
                         << "十分之一周期" << "平均波高" << "平均周期" << "三分之一周期"
                         << "区域中心值" << "水深" << "峰波波长";

                    //添加Excel表头数据
                    for(int j = 1; j <= 20 ; j++)
                    {
                        cell = worksheet->querySubObject("Cells(int,int)", 1, j);
                        cell->setProperty("RowHeight", 40);
                        cell->dynamicCall("SetValue(const QString&)",list[j-1]);
                    }
                    /*QChar ch = 20 + 0x40; //A对应0x41
                    QString res = QString(ch);
                    res += QString::number(i+2);
                    res = "A2:" + res;
                    QAxObject *range = worksheet->querySubObject("Range(const QString&)",res); //设定写入excel中的表格范围
                    if(range == nullptr || range->isNull())
                    {
                        return;
                    }
                    //QVariant v = QVariant(saveData);
                    range->setProperty("Value",saveData);
                    delete range;*/

                    QAxObject *range;
                    for(int n=0;n<ui->tab->rowCount();n++)
                    {
                        //res = nullptr;
                        QChar ch = 20 + 0x40; //A对应0x41
                        QString res = QString(ch);
                        res += QString::number(n+2);
                        res = "A" + QString::number(n+2) + ":" + res;
                        range = worksheet->querySubObject("Range(const QString&)",res); //设定写入excel中的表格范围
                        if(range == nullptr || range->isNull())
                        {
                            return;
                        }
                        //QVariant v = QVariant(saveData);
                        range->setProperty("Value",allData.at(n));
                    }
                    delete range;
                    //将生成的Excel文件保存到指定目录下
                    workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName)); //保存至fileName
                    workbook->dynamicCall("Close()");                                                   //关闭工作簿
                    excel->dynamicCall("Quit()");                                                       //关闭excel
                    delete excel;
                    excel = NULL;
                }
            }
            qDebug() << "excel文件创建完毕";
        }
    }
    /*if(i == 0)
    {
        tempData = new double[ui->tab->rowCount()*16];
        memcpy(&tempData[i*16],d,16*sizeof (double));
        for(int k=0;k<16;k++)
        {
            qDebug() << "aaaaaaa1" << tempData[i*16+k] << ui->tab->rowCount();
        }
    }
    else
    {
        memcpy(&tempData[i*16],d,16*sizeof (double));
        for(int k=0;k<16;k++)
        {
            qDebug() << "aaaaaaa2" << tempData[i*16+k];
        }
    }
    if(i == ui->tab->rowCount() - 1)
    {
        QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        int check = ini->value("/batchprocess/xlsx").toInt();
        if(check)
        {
            fileName = ui->excelPath->text() + "\\" + ui->excelName->text() + ".xlsx";    //设置可选择的保存的文件名xls或txt文件
            fileName.replace(QString("\\"), QString("/"));
            qDebug() << "filename" << fileName;
            if(fileName != NULL) //xls文件为空的时候创建xls文件
            {
                file1.setFileName(ui->excelName->text() + ".xlsx");
                QAxObject *excel = new QAxObject;
                if(excel->setControl("Excel.Application")) //连接Excel控件
                {
                    excel->dynamicCall("SetVisible (bool Visible)",false); //不显示窗体
                    excel->setProperty("DisplayAlerts",false); //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
                    QAxObject *workbooks = excel->querySubObject("WorkBooks");            //获取工作簿集合
                    workbooks->dynamicCall("Add");                                        //新建一个工作簿
                    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");        //获取当前工作簿
                    QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
                    QAxObject *cell;

                    //添加Excel表头数据
                    QStringList list;
                    list << "时间" << "文件名称" << "经度" << "纬度"
                         << "有效波高" << "峰波周期" << "峰波频率" << "峰波波向"
                         << "流速" << "流向" << "信噪比" << "零值矩阵" << "十分之一波高"
                         << "十分之一周期" << "平均波高" << "平均周期" << "周期"
                         << "区域中心值" << "水深" << "峰波波长";
                    for(int i = 1; i <= 20 ; i++)
                    {
                        cell = worksheet->querySubObject("Cells(int,int)", 1, i);
                        cell->setProperty("RowHeight", 40);
                        cell->dynamicCall("SetValue(const QString&)",list[i-1]);
                    }
                    for(int i=0;i<ui->tab->rowCount();i++)
                    {
                        for(int j=0;j<20;j++)
                        {
                            if(j == 0)
                            {
                                cell = worksheet->querySubObject("Cells(int,int)",i+2,1);
                                cell->dynamicCall("SetValue(const QString&)",d9 + "\t");
                            }
                            else if(j == 1)
                            {
                                //cell = worksheet->querySubObject("Cells(int,int)", i, 0);
                                cell = worksheet->querySubObject("Cells(int,int)",i+2,2);
                                QString temp = ui->tab->item(i,0)->text();
                                QString save = temp.left(temp.indexOf(" "));
                                cell->dynamicCall("SetValue(const QString&)",save + "\t");

                            }
                            else if(j == 2)
                            {
                                cell = worksheet->querySubObject("Cells(int,int)",i+2,3);
                                cell->dynamicCall("SetValue(const QString&)",QString::number(d6) + "\t");
                            }
                            else if(j == 3)
                            {
                                cell = worksheet->querySubObject("Cells(int,int)",i+2,4);
                                cell->dynamicCall("SetValue(const QString&)",QString::number(d7) + "\t");
                            }
                            else
                            {
                                cell = worksheet->querySubObject("Cells(int,int)",i+2,j+1);
                                cell->dynamicCall("SetValue(const QString&)",QString::number(tempData[j-4]) + "\t");
                            }
                        }
                        if(i==0)
                        {
                            for(int k=0;k<16;k++)
                                qDebug() << "查看" << tempData[k];
                        }
                    }
                    //将生成的Excel文件保存到指定目录下
                    workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName)); //保存至fileName
                    workbook->dynamicCall("Close()");                                                   //关闭工作簿
                    excel->dynamicCall("Quit()");                                                       //关闭excel
                    delete excel;
                    excel = NULL;
                    qDebug() << "excel输出";
                }
            }
            delete [] tempData;
        }
    }*/
}

//导出按钮的槽函数
void batchproc::export_Slot()
{
    /*QString fileName = QFileDialog::getSaveFileName(this,tr("Save files"),"","Excel Files(*.xlsx);;Text files(*.txt)");    //设置可选择的保存的文件名xls或txt文件
    if(fileName != NULL) //xls文件为空的时候创建xls文件
    {
        QAxObject *excel = new QAxObject;
        if(excel->setControl("Excel.Application")) //连接Excel控件
        {
            excel->dynamicCall("SetVisible (bool Visible)",false); //不显示窗体
            excel->setProperty("DisplayAlerts",false); //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            QAxObject *workbooks = excel->querySubObject("WorkBooks");            //获取工作簿集合
            workbooks->dynamicCall("Add");                                        //新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");        //获取当前工作簿
            QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
            QAxObject *cell;
            tab->rowCount();
            int columnCount = tab->columnCount();

            //添加Excel表头数据
            for(int i = 1; i <= columnCount ; i++)
            {
                cell = worksheet->querySubObject("Cells(int,int)", 1, i);
                cell->setProperty("RowHeight", 40);
                cell->dynamicCall("SetValue(const QString&)",tab->horizontalHeaderItem(i-1)->data(0).toString());
            }
            //将form列表中的数据依此保存到Excel文件中
            for(int j = 2; j <= row + 1;j++)
            {
                for(int k = 1;k <= tab->columnCount();k++)
                {
                    cell = worksheet->querySubObject("Cells(int,int)", j, k);
                    if(k == 1)
                    {
                        QString temp = tab->item(j-2,k-1)->text();
                        QString save = temp.left(temp.indexOf(" "));
                        cell->dynamicCall("SetValue(const QString&)",save + "\t");
                    }
                    else
                    {
                        cell->dynamicCall("SetValue(const QString&)",tab->item(j-2,k-1)->text() + "\t");
                    }
                }
            }
            //将生成的Excel文件保存到指定目录下
            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName)); //保存至fileName
            workbook->dynamicCall("Close()");                                                   //关闭工作簿
            excel->dynamicCall("Quit()");                                                       //关闭excel
            delete excel;
            excel = NULL;
            if (QMessageBox::question(NULL,QString::fromUtf8("完成"),QString::fromUtf8("文件已经导出，是否现在打开？"),QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
            }
        }
    }
    qDebug() << "读完了";*/
    QString fileName = ui->excelPath->text() + ui->excelName->text();    //设置可选择的保存的文件名xls或txt文件
    qDebug() << "filename" << fileName;
    if(fileName != NULL) //xls文件为空的时候创建xls文件
    {
        QAxObject *excel = new QAxObject;
        if(excel->setControl("Excel.Application")) //连接Excel控件
        {
            excel->dynamicCall("SetVisible (bool Visible)",false); //不显示窗体
            excel->setProperty("DisplayAlerts",false); //不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            QAxObject *workbooks = excel->querySubObject("WorkBooks");            //获取工作簿集合
            workbooks->dynamicCall("Add");                                        //新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");        //获取当前工作簿
            QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
            //将生成的Excel文件保存到指定目录下
            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName)); //保存至fileName
            workbook->dynamicCall("Close()");                                                   //关闭工作簿
            excel->dynamicCall("Quit()");                                                       //关闭excel
            delete excel;
            excel = NULL;
            if (QMessageBox::question(NULL,QString::fromUtf8("完成"),QString::fromUtf8("文件已经导出，是否现在打开？"),QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
            }
        }
    }
    QAxObject *excel = new QAxObject;
    QAxObject *workbooks = excel->querySubObject("WorkBooks");    //获取工作簿集合
    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");        //获取当前工作簿
    QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);

}

//关闭界面按钮的槽函数
void batchproc::close_Slot()
{
    QSettings *ini = new QSettings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    int check = ui->check_xlsx->isChecked();
    ini->setValue("/batchprocess/xlsx",check);
    ini->setValue("/batchprocess/xlsxPath",ui->excelPath->text());
    check = ui->check_txt->isChecked();
    ini->setValue("/batchprocess/txt",check);
    ini->setValue("/batchprocess/txtPath",ui->txtPath->text());
    ini->setValue("/batchprocess/xlsxName/",ui->excelName->text()); //excel文件名称
    //ui->tab->clearContents(); //删除除表头外的所有内容
    //ui->tab->setRowCount(0); //行数设置为0
    int count = ui->tab->rowCount();
    for(int i=0;i<count;i++)
        ui->tab->removeRow(0);
    qDebug() << "结束";
    qDebug() << "结束";
    this->close();
}

batchproc::~batchproc()
{
    delete ui;
}
