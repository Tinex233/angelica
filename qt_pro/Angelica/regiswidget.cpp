#include "regiswidget.h"
#include "ui_regiswidget.h"

regisWidget::regisWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::regisWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    //窗口的删除任务交给父类
    setAttribute(Qt::WA_DeleteOnClose, false);

    ui->password->setEchoMode(QLineEdit::Password);
    ui->rePassword->setEchoMode(QLineEdit::Password);
    setWindowIcon(QIcon(":/new/icon/resources/Angelica+.ico"));
    draggableArea.setRect(0,0,450,35);
}

regisWidget::~regisWidget()
{
    delete ui;
}

void regisWidget::on_regis_btn_clicked()
{

    if(ui->password->text()!=ui->rePassword->text())
    {
        QMessageBox::warning(this,"提示","两次密码不一致",QMessageBox::Ok);
        return;
    }

    if(ui->name->text().isEmpty())
    {
        QMessageBox::warning(this,"提示","请输入用户名",QMessageBox::Ok);
        return;
    }

    if(ui->password->text().isEmpty())
    {
        QMessageBox::warning(this,"提示","请输入密码",QMessageBox::Ok);
        return;
    }

    if(ui->rePassword->text().isEmpty())
    {
        QMessageBox::warning(this,"提示","请再次输入密码",QMessageBox::Ok);
        return;
    }

    QNetworkAccessManager* manager = new QNetworkAccessManager();

     // 构建请求 URL
     QUrl url("http://localhost:5000/api/auth/register");
     QNetworkRequest request(url);

     // 设置请求头
     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

     // 构建 JSON 请求体
     QJsonObject json;
     json["user_name"] = ui->name->text();  // 使用用户名字段
     json["password"] = ui->password->text();   // 使用密码字段

     // 转换为 JSON 文本
     QByteArray requestBody = QJsonDocument(json).toJson();

     // 发送 POST 请求
     QNetworkReply* reply = manager->post(request, requestBody);

     // 处理服务器响应
     QObject::connect(reply, &QNetworkReply::finished, [=]() {
         if (reply->error() == QNetworkReply::NoError) {
             // 读取响应数据
             QByteArray responseData = reply->readAll();
             QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);

             // 检查是否是 JSON 对象
             if (responseDoc.isObject()) {
                 QJsonObject jsonObj = responseDoc.object();

                 // 访问 JSON 对象中的 "message" 和 "user_id" 键
                 if (jsonObj.contains("message") && jsonObj["message"].isString()) {
                     QString message = jsonObj["message"].toString();
                     qDebug() << "Registration successful:" << message;

                     // 获取新用户的 user_id
                     int userId = jsonObj.value("user_id").toInt();
                     //qDebug() << "New User ID:" << userId;
                     QMessageBox::warning(this, "提示", "注册成功，账号："+QString::number(userId), QMessageBox::Ok);
                     //qDebug()<<userId;
                     emit returnToLogin();
                 } else {
                     qDebug() << "Unexpected response format or missing 'message' key.";
                     QMessageBox::warning(this, "提示", "注册失败", QMessageBox::Ok);
                 }
             }
             else
             {
                 qDebug() << "Response is not a valid JSON object.";
             }
         } else {
             // 处理错误
             qDebug() << "Error:" << reply->errorString();
             QMessageBox::warning(this, "提示", "请检查网络设置", QMessageBox::Ok);
         }
         reply->deleteLater();
     });
}

void regisWidget::on_close_btn_clicked()
{
    emit returnToLogin();
    ui->name->clear();
    ui->password->clear();
    ui->rePassword->clear();
}

void regisWidget::on_min_btn_clicked()
{
    this->showMinimized();
}

void regisWidget::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton) { // 检查是否按下了鼠标左键
        isDragging = true; // 设置拖动状态为真，表示开始拖动
        dragPosition = event->globalPos() - frameGeometry().topLeft(); // 记录鼠标按下时的全局位置与窗口左上角位置的差值
        event->accept(); // 接受事件，防止事件进一步传播
    }
    return QWidget::mousePressEvent(event); // 调用基类的 mousePressEvent 以确保默认行为被执行
}

void regisWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!draggableArea.contains(event->pos()))
        return;

    if (isDragging && (event->buttons() & Qt::LeftButton)) { // 检查是否正在拖动并且左键仍然按下
        move(event->globalPos() - dragPosition); // 计算并设置窗口的新位置
        event->accept(); // 接受事件，防止事件进一步传播
    }
    return QWidget::mouseMoveEvent(event); // 调用基类的 mouseMoveEvent 以确保默认行为被执行
}

void regisWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) { // 检查是否释放了鼠标左键
        isDragging = false; // 设置拖动状态为假，表示停止拖动
        event->accept(); // 接受事件，防止事件进一步传播
    }
    return QWidget::mouseReleaseEvent(event); // 调用基类的 mouseReleaseEvent 以确保默认行为被执行
}
