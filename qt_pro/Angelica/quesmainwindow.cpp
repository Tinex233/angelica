#include "quesmainwindow.h"
#include "ui_quesmainwindow.h"

quesMainWindow::quesMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::quesMainWindow)
{
    ui->setupUi(this);
    setWindowTitle("中医体质分类与判定表");
    setWindowIcon(QIcon(":/new/icon/resources/Angelica+.ico"));
    setWindowFlags(Qt::Dialog);//设为对话框类型
    setWindowModality(Qt::ApplicationModal);//应用级模态（该类设计为继承QDialog更好，可以使用exec有效阻塞主窗口，不必绑定销毁事件以获取数据）

    numOfPage=ui->stackedWidget->count();
    setPageLabel(1,numOfPage);

    //添加表格样式
    for(int i=0;i<ui->stackedWidget->count();i++)
    {
        QTableWidget *tw = ui->stackedWidget->widget(i)->findChild<QTableWidget*>("tableWidget_"+QString::number(i+1));

        if(tw)
            setTabWidStyle(tw,tw->rowCount(),tw->columnCount(),i);
    }

    res="";

    ui->pre_btn->setDisabled(true);

    cons[0]="平和质";
    cons[1]="气虚质";
    cons[2]="阳虚质";
    cons[3]="阴虚质";
    cons[4]="痰湿质";
    cons[5]="湿热质";
    cons[6]="血瘀质";
    cons[7]="气郁质";
    cons[8]="特禀质";

}

void quesMainWindow::setTabWidStyle(QTableWidget *tw, int row, int col,int index)
{
    //单元格展开所有内容
    tw->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //表头行样式
    QHeaderView *horizontalHeader = tw->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::Fixed); // 第一列设为固定宽度
    horizontalHeader->resizeSection(0, 500);
    horizontalHeader->setStretchLastSection(false);//禁止拉伸

    //添加radio button
    QString mystyle="QRadioButton{"
            "font: 11pt \"微软雅黑\"}"
            "QRadioButton::indicator{"
            "width:22px;"
            "height:22px;}"
            "QRadioButton::indicator::unchecked {"
            "border-image: url(:/new/setting/resources/setting/radio-button-22.png);}"
            "QRadioButton::indicator::checked{"
            "    border-image:url(:/new/setting/resources/setting/radio-button-22-checked.png);}";

    for(int i=0;i<row;i++)
    {
        btnGroup[index].push_back(new QButtonGroup(tw));
        int id=1;
        for(int j=1;j<col;j++)
        {
            QWidget *wg=new QWidget(tw);
            QHBoxLayout *hl=new QHBoxLayout();
            QRadioButton *rb=new QRadioButton();

            rb->setStyleSheet(mystyle);
            btnGroup[index][i]->addButton(rb,id++);
            rb->setAutoExclusive(true);
            hl->addWidget(rb);
            hl->setAlignment(rb,Qt::AlignCenter);
            wg->setLayout(hl);
            tw->setCellWidget(i,j,wg);
        }

    }
}

quesMainWindow::~quesMainWindow()
{
    delete ui;
}

void quesMainWindow::on_close_btn_clicked()
{
    this->close();
}

//上一页按钮
void quesMainWindow::on_pre_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(--curPage);

    if(curPage==0)
        ui->pre_btn->setDisabled(true);

    if(curPage+1<ui->stackedWidget->count())
        ui->next_btn->setEnabled(true);

    setPageLabel(curPage+1,numOfPage);
}

//下一页按钮
void quesMainWindow::on_next_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(++curPage);

    if(curPage+1==ui->stackedWidget->count())
        ui->next_btn->setDisabled(true);

    if(curPage>0)
        ui->pre_btn->setEnabled(true);

    setPageLabel(curPage+1,numOfPage);
}

void quesMainWindow::setPageLabel(int cur, int sum)
{
    ui->page_label->setText(" — 第"+QString::number(cur)+"页，共"+QString::number(sum)+"页 — ");
}

//提交按钮
void quesMainWindow::on_sub_btn_clicked()
{
    int grade[9];

    //第一张表有特殊评分特殊处理，具体见参考论文
    grade[0]=0;
    for(int i=0;i<btnGroup[0].size();i++)
    {
        int id=btnGroup[0][i]->checkedId();
        if(id==-1)
        {
            QMessageBox::warning(this,"提示","第1页存在问题未回答！",QMessageBox::Ok);
            return;
        }

        if(i==0||i==5)
            grade[0]+=id;
        else
            grade[0]+=6-id;
    }

    for(int i=1;i<9;i++)
    {
        grade[i]=0;
        for(int j=0;j<btnGroup[i].size();j++)
        {
            int id=btnGroup[i][j]->checkedId();
            if(id==-1)
            {
                QMessageBox::warning(this,"提示","第"+QString::number(i+1)+"页存在问题未回答！",QMessageBox::Ok);
                return;
            }
            grade[i]+=id;
        }
    }

    //根据分数判断体质
    int num[9];//问题条目数
    num[0]=num[1]=num[3]=num[4]=8;
    num[2]=num[6]=num[7]=num[8]=7;
    num[5]=6;

    bool below_30=true,below_40=true;

    for(int i=0;i<9;i++)
    {
        grade[i]=((grade[i]-num[i])*100)/(num[i]*4);
        //qDebug()<<grade[i]<<" ";
        if(i>0)
        {
            if(grade[i]>=30&&grade[i]<40)
            {
                below_30=false;
                res+="有"+cons[i]+"倾向 ";
            }
            else if(grade[i]>=40)
            {
                below_30=below_40=false;
                res+="是"+cons[i]+" ";
            }
        }
    }

    if(grade[0]>=60)
    {
        if(below_30)
            res="是平和质 "+res;
        else if(below_40)
            res="基本是平和质 "+res;
    }

    QMessageBox::warning(this,"测试结果","测试结果："+res,QMessageBox::Ok);

    this->close();
}

QString quesMainWindow::getResult()
{
    return res;
}
