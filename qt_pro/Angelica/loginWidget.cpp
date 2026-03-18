#include "loginWidget.h"
#include "ui_loginWidget.h"

loginWidget::loginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loginWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_QuitOnClose);
    ui->password->setEchoMode(QLineEdit::Password);
    setWindowIcon(QIcon(":/new/icon/resources/Angelica+.ico"));
    draggableArea.setRect(0,0,450,30);

    m_manager=new QNetworkAccessManager(this);

    w=new MainWindow();
    w->hide();

    r=new regisWidget();
    r->hide();
    connect(w,&MainWindow::logout,this,[=]{
        // 从本地存储获取 Token
        QSettings settings;
        QString token = settings.value("auth/token").toString();
        if (token.isEmpty()) {
            QMessageBox::warning(this,"提示","未找到登录凭证",QMessageBox::Ok);
            return;
        }

        // 构建请求
        QNetworkRequest request;
        request.setUrl(QUrl("http://localhost:5000/api/auth/logout"));
        request.setRawHeader("Authorization",
                           QString("Bearer %1").arg(token).toUtf8());

        // 发送 DELETE 请求
        QNetworkReply *reply = m_manager->deleteResource(request);

       connect(reply, &QNetworkReply::finished, [=]{
           // 处理响应
           handleLogoutResponse(reply);
           reply->deleteLater();
       });
    });

    //从注册页面转回登录页面
    connect(r,&regisWidget::returnToLogin,this,[=]{
        this->ui->account->clear();
        this->ui->password->clear();
        this->setHidden(false);
        r->hide();
    });

    connect(w,&MainWindow::closeWindow,this,[=]{
       QApplication::quit();
       this->close();
    });
}

void loginWidget::handleLogoutResponse(QNetworkReply *reply)
{
    // 网络层错误处理
    if (reply->error() != QNetworkReply::NoError) {
        QString errorStr = reply->errorString();
        if (reply->error() == QNetworkReply::OperationCanceledError) {
            errorStr = "请求超时，请检查网络连接";
        }
        QMessageBox::warning(this,"提示","网络错误："+errorStr,QMessageBox::Ok);
        return;
    }

    // 解析 HTTP 状态码
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray responseData = reply->readAll();

    // 成功处理 (2xx 状态码)
    if (statusCode >= 200 && statusCode < 300) {
        clearLocalAuthData();
        this->ui->password->clear();
        this->setHidden(false);
        w->hide();
        return;
    }

    // 解析错误信息
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    QString errorMsg = "未知错误";

    if (parseError.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();
        if (json.contains("error")) {
            errorMsg = json["error"].toString();
        }
    }

    // 特定状态码处理
    switch (statusCode) {
    case 401:
        errorMsg = "会话已过期，请重新登录";
        clearLocalAuthData();
        break;
    case 403:
        errorMsg = "没有操作权限";
        break;
    case 500:
        errorMsg = "服务器内部错误";
        break;
    }

    QMessageBox::warning(this,"提示",errorMsg,QMessageBox::Ok);
}

void loginWidget::clearLocalAuthData()
{
    QSettings settings;
    settings.remove("auth/token");
    settings.remove("auth/user_id");
    settings.sync();
}

loginWidget::~loginWidget()
{
    delete w;
    delete r;
    delete ui;
}

void loginWidget::on_close_btn_clicked()
{
    this->close();
}

void loginWidget::on_login_btn_clicked()
{
    if(ui->account->text().isEmpty())
    {
        QMessageBox::warning(this,"提示","请输入账号",QMessageBox::Ok);
        return;
    }

    if(ui->password->text().isEmpty())
    {
        QMessageBox::warning(this,"提示","请输入密码",QMessageBox::Ok);
        return;
    }
    QNetworkAccessManager* manager = new QNetworkAccessManager();

     // 构建请求 URL
     QUrl url("http://localhost:5000/api/auth/login");
     QNetworkRequest request(url);

     // 设置请求头
     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

     // 构建 JSON 请求体
     QJsonObject json;
     json["user_id"] = ui->account->text();
     json["password"] = ui->password->text();

     // 转换为 JSON 文本
     QByteArray requestBody = QJsonDocument(json).toJson();

     // 发送 POST 请求
     QNetworkReply* reply = manager->post(request, requestBody);

     // 处理服务器响应
     QObject::connect(reply, &QNetworkReply::finished, [=]() {
         if (reply->error() == QNetworkReply::NoError)
         {
             // 读取响应数据
             QByteArray responseData = reply->readAll();
             QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
             // 检查是否是 JSON 对象
             if (responseDoc.isObject()) {
                 QJsonObject jsonObj = responseDoc.object();

                 // 访问 JSON 对象中的 "message" 键
                 if (jsonObj.contains("message") && jsonObj["message"].isString()) {
                     QString message = jsonObj["message"].toString();
                     qDebug() << message;

                     w->setUserID(ui->account->text().toInt());
                     QString token = jsonObj["access_token"].toString();
                     w->setToken(token);
                     //qDebug()<<jsonObj["access_token"].toString();
//                     QSettings settings;
//                     settings.setValue("auth/token", token);
//                     settings.setValue("auth/user_id", jsonObj["user_id"].toInt());
//                     settings.sync();
                     //qDebug() << "Settings path:" << settings.fileName();

                     this->hide();
                     w->setHidden(false);
//                     close();
//                     MainWindow *w=new MainWindow();
//                     w->setUserID(ui->account->text().toInt());
//                     w->setWindowTitle("芷宁 Angelica+");
//                     w->setWindowIcon(QIcon(":/new/icon/resources/Angelica+.ico"));//设置程序图标
//                     w->show();
                 }
                 else
                 {
                     qDebug() << "Unexpected response format or missing 'message' key.";
                     QMessageBox::warning(this,"提示","密码错误",QMessageBox::Ok);
                 }
             }
             else
             {
                 qDebug() << "Response is not a valid JSON object.";
             }
         }
         else
         {
             // 处理错误
             qDebug() << "Error:" << reply->errorString();
             if(reply->errorString().contains("NOT FOUND"))
                 QMessageBox::warning(this,"提示","无效账号",QMessageBox::Ok);
             else if(reply->errorString().contains("Host"))
             {
                 QMessageBox::warning(this,"提示","密码错误，请重新输入",QMessageBox::Ok);
                 ui->password->clear();
             }
             else
                 QMessageBox::warning(this,"提示","请检查网络设置",QMessageBox::Ok);
         }
         reply->deleteLater();
     });

}

void loginWidget::on_register_btn_clicked()
{
    r->setHidden(false);
    this->hide();
}

void loginWidget::on_min_btn_clicked()
{
    this->showMinimized();
}

void loginWidget::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton)
    {
        isDragging = true;
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    return QWidget::mousePressEvent(event);
}

void loginWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!draggableArea.contains(event->pos()))
        return;

    if (isDragging && (event->buttons() & Qt::LeftButton))
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
    return QWidget::mouseMoveEvent(event);
}

void loginWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isDragging = false;
        event->accept();
    }
    return QWidget::mouseReleaseEvent(event);
}
