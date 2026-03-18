#include "registerwidget.h"
#include "ui_registerwidget.h"

registerWidget::registerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registerWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    ui->password->setEchoMode(QLineEdit::Password);
    setWindowIcon(QIcon(":/new/icon/resources/Angelica+.ico"));
}

registerWidget::~registerWidget()
{
    delete ui;
}
