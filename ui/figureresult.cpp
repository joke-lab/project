#include "figureresult.h"
#include "ui_figureresult.h"

figureresult::figureresult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::figureresult)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    QObject::connect(ui->search,SIGNAL(clicked(bool)),this,SLOT(search_Slot()));
    QObject::connect(ui->expor,SIGNAL(clicked(bool)),this,SLOT(export_Slot()));
    QObject::connect(ui->clo,SIGNAL(clicked(bool)),this,SLOT(close_Slot()));

}

//查询按钮的槽函数
void figureresult::search_Slot()
{
    QString tempT = ui->startTime->text(); //获取开始时间
    QString startT = ui->startTime->text();
    int yearS = startT.mid(0,4).toInt(); //获取查询的起始年份
    QString endT = ui->endTime->text();
    int yearE = endT.mid(0,4).toInt(); //获取查询的截止年份
    /*for( int i=0;i<tempT.length();i++)
    {
        if(tempT[i]>='0' && tempT[i] < '9')
            startT.append(tempT[i]);
    }
    tempT = endTime->text();
    QString endT = 0; //获取结束时间
    for( int i=0;i<tempT.length();i++)
    {
        if(tempT[i]>='0' && tempT[i] < '9')
            endT.append(tempT[i]);
    }*/
    qDebug() << "开始结束时间" << startT << endT;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbTemp");
    //QSqlQuery query(db);//连接数据库
    if (!db.open())
    {
        QMessageBox::information(nullptr,"数据库连接失败","SQLite数据库连接失败，请检查软件版本并重新启动",1,QMessageBox::Ok);
    }
    QSqlQuery query(db);
    //query.prepare("SELECT * FROM tab9l WHERE id = ?");
    //query.addBindValue(mInputText->text().toInt());

    /*while (query.next())
    {
        qDebug() << query.value(0).toString()
                 << query.value(1).toString();
    }*/
    for(int year=yearS;year<=yearE;year++)
    {
        QString SqlStr = QString("select * from [%1] where time between '%2' and '%3'").arg("2020").arg(startT).arg(endT);
        query.exec(SqlStr);
        while(query.next())
        {
                int row = ui->table->rowCount();
                ui->table->insertRow(row);
                qDebug() <<"行数" << row << ui->table->rowCount();
                ui->table->setItem(row,0,new QTableWidgetItem(query.value(1).toString()));
                ui->table->setItem(row,1,new QTableWidgetItem(query.value(2).toString()));
                ui->table->setItem(row,2,new QTableWidgetItem(query.value(3).toString()));
                ui->table->setItem(row,3,new QTableWidgetItem(QString::number(query.value(4).toFloat(),'f',4)));
                ui->table->setItem(row,4,new QTableWidgetItem(QString::number(query.value(5).toFloat(),'f',4)));
                ui->table->setItem(row,5,new QTableWidgetItem(QString::number(query.value(6).toFloat(),'f',4)));
                ui->table->setItem(row,6,new QTableWidgetItem(QString::number(query.value(7).toFloat(),'f',4)));
                ui->table->setItem(row,7,new QTableWidgetItem(QString::number(query.value(8).toFloat(),'f',4)));
                ui->table->setItem(row,8,new QTableWidgetItem(QString::number(query.value(9).toFloat(),'f',4)));
                ui->table->setItem(row,9,new QTableWidgetItem(query.value(0).toString()));
          }
    }
    int rowCount = ui->table->rowCount();
    tempData = new float[20*rowCount];
    tempTimeName.clear();
    for(int year=yearS;year<=yearE;year++)
    {
        QString SqlStr = QString("select * from [%1] where time between '%2' and '%3'").arg("2020").arg(startT).arg(endT);
        query.exec(SqlStr);
        while(query.next())
        {
            for(int i=0;i<rowCount;i++)
            {
                for(int j=0;j<20;j++)
                {
                    if(j==0 || j==1)
                        tempTimeName << query.value(j).toString();
                    else
                        tempData[i*20+j] = query.value(j).toFloat(nullptr);
                }
            }
        }
    }
}
    //qDebug() <<query
    //query.addBindValue(mInputText->text().toInt());
    //select * from sqlite_master where table_name like '%20201214%' ;//sqlite_master是所有表名，like是模糊查询，20201214为模糊表名（从所有表名中含有20201214的非空表名）
    //QDate datetimeEdit = startTime->date();
    //QString date = datetimeEdit.toString("yyyy-MM-dd hh:mm:ss");
    //qDebug()<<date;

    /*if(!query.exec(QString("SELECT ID FROM sqlite_master where datetime(name) >= '%1' and datetime(name) <= '%2' ").arg(startT).arg(endT)))
        qWarning() << "MainWindow::OnSearchClicked - ERROR: " << query.lastError().text();

    qDebug() << query.first() << query.next() << query.nextResult() << query.last();*/
    /*if(query.first())
        mOutputText->setText(query.value(0).toString());
    else if(query.next())
    {
        mOutputText->setText("bababa");
    }
        //mOutputText->setText(query.value(1).toString());
    else
        mOutputText->setText("person not found");*/



//导出按钮的槽函数
void figureresult::export_Slot()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save files"),"","Excel Files(*.xlsx);;Text files(*.txt)");    //设置可选择的保存的文件名xls或txt文件
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
            int rowCount = ui->table->rowCount();
            /*添加Excel表头数据*/
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
            for(int i=0;i<rowCount;i++)
            {
                for(int j=0;j<20;j++)
                {
                    if(j==0 || j==1)
                    {
                        cell = worksheet->querySubObject("Cells(int,int)", i+2, j+1);
                        cell->dynamicCall("SetValue(const QString&)",tempTimeName.at(i*2+j) + "\t");
                    }
                    else
                    {
                        cell = worksheet->querySubObject("Cells(int,int)", i+2, j+1);
                        cell->dynamicCall("SetValue(const QString&)",QString::number(tempData[i*20+j]) + "\t");
                    }
                }
            }
            delete [] tempData; //注释掉临时数组

            //将form列表中的数据依此保存到Excel文件中
           /* for(int j=2;j<=table->rowCount()+1;j++)
            {
                for(int k=1;k<=columnCount;k++)
                {
                    cell = worksheet->querySubObject("Cells(int,int)", j, k);
                    cell->dynamicCall("SetValue(const QString&)",table->item(j-2,k-1)->text() + "\t");
                }
            }*/
            //将生成的Excel文件保存到指定目录下
            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName)); //保存至fileName
            workbook->dynamicCall("Close()"); //关闭工作簿
            excel->dynamicCall("Quit()"); //关闭excel
            delete excel;
            excel = NULL;
            if (QMessageBox::question(NULL,QString::fromUtf8("完成"),QString::fromUtf8("文件已经导出，是否现在打开？"),QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
            }
        }
    }
}

//关闭按钮的槽函数
void figureresult::close_Slot()
{
    row = 0; //将现有的数据清除
    this->close();
}

figureresult::~figureresult()
{
    delete ui;
}
