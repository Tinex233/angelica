#include "appointmentwidget.h"
#include "ui_appointmentwidget.h"

appointmentWidget::appointmentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::appointmentWidget)
{
    ui->setupUi(this);
    setWindowTitle("预约详细信息");
    setWindowIcon(QIcon(":/new/icon/resources/Angelica+.ico"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
}

void appointmentWidget::addData(const appointmentDetail &a)
{
    id=a.appoID;

    ui->patName->setText(a.patName);
    ui->idType->setText(a.idType);
    ui->idNum->setText(a.idNum);
    ui->patPhone->setText(a.phone);
    ui->patCons->setText(a.cons);

    ui->hospitalName->setText(a.hos);
    ui->hosPos->setText(a.hosPos);

    ui->doctor->setText(a.docName);
    ui->appoDate->setText(a.appoDate);

    ui->desc->setText(a.desc);

    ui->price->setText("￥"+a.price);
    ui->createDate->setText(a.createTime);

    ui->status->setText(a.status);
    if(a.status!="待就诊")
        ui->cancel_btn->hide();
    if(a.status=="已完成")
        ui->status->setStyleSheet("font: 10pt \"微软雅黑\";color: rgb(0, 170, 0);");
    else if(a.status=="待就诊")
        ui->status->setStyleSheet("font: 10pt \"微软雅黑\";color: rgb(255, 0, 0);");
    else if(a.status=="已取消")
        ui->status->setStyleSheet("font: 10pt \"微软雅黑\";color: rgb(0, 170, 255);");
    else if(a.status=="已爽约")
        ui->status->setStyleSheet("font: 10pt \"微软雅黑\";color: rgb(255, 170, 0);");

    connect(ui->cancel_btn,&QPushButton::clicked,this,[=]{
        emit cancel(id);
    });
}

appointmentWidget::~appointmentWidget()
{
    delete ui;
}
