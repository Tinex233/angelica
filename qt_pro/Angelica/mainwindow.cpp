#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //窗口的删除任务交给父类
    setAttribute(Qt::WA_DeleteOnClose, false);

    setWindowTitle("芷宁 Angelica+");
    setWindowIcon(QIcon(":/new/icon/resources/Angelica+.ico"));//设置程序图标
    ui->credit_label->setToolTip("本月可允许0次预约未到");

    firstOpenAi=true;

    //初始化用户按钮样式
    user_style=
            "background-color: rgb(255, 255, 255);"
            "border-radius: 25px;"
            "font-size: 20px;"
            "height:50px;"
            "width:50px;";
    user_head=":/new/user/resources/head/icons8-man-48.png";
    ui->user->setStyleSheet(user_style+"border:1px solid #999999;");
    ui->user->setIcon(QIcon(QPixmap(user_head)));
    ui->user->setIconSize(ui->user->size()*0.8);

    initBtnStyle();

    QGraphicsDropShadowEffect *sd1 = new QGraphicsDropShadowEffect(this);
    sd1->setOffset(4, 4);//阴影的偏移量
    sd1->setColor(QColor(43, 43, 43));//阴影的颜色
    sd1->setBlurRadius(8); // 阴影圆角的大小
    ui->frame_2->setGraphicsEffect(sd1);

    //RadioButton互斥
//    QButtonGroup *setRbGroup = new QButtonGroup(this);
//    setRbGroup->addButton(ui->setMesOpen_btn);
//    setRbGroup->addButton(ui->setMesClose_btn);
//    ui->setMesOpen_btn->setAutoExclusive(true);
//    ui->setMesClose_btn->setAutoExclusive(true);

    leftBtnGroup[0]=ui->home_btn;
    leftBtnGroup[1]=ui->record_btn;
    leftBtnGroup[2]=ui->ai_btn;
    leftBtnGroup[3]=ui->setting_btn;
    leftBtnGroup[4]=ui->static_btn;

    recBtnGroup[0]=ui->allRec_btn;
    recBtnGroup[1]=ui->waitRec_btn;
    recBtnGroup[2]=ui->finRec_btn;
    recBtnGroup[3]=ui->canRec_btn;
    recBtnGroup[4]=ui->breakRec_btn;

    //主页
    sw1=new scaleWidget(ui->ratingWidget);
    sw1->setText("不痒","可想象的\n最痒");
    connect(sw1,&scaleWidget::scoreSelected,this,[=](int score){
        qDebug()<<score;
        ui->itchGrade_label->setText(QString::number(score));
    });

    sw2=new scaleWidget(ui->ratingWidget_2);
    sw2->setText("不疼","可想象的\n最疼");
    connect(sw2,&scaleWidget::scoreSelected,this,[=](int score){
        qDebug()<<score;
        ui->painGrade_label->setText(QString::number(score));
    });
    ui->subItch_btn->setText("提交");
    ui->subPain_btn->setText("提交");
    ui->subItch_btn->setEnabled(true);
    ui->subPain_btn->setEnabled(true);

    //智能助手区块
    QNChatMessage().setMyHead(user_head);
    QNChatMessage().setAIHead(":/new/icon/resources/Angelica+.ico");

    aiConsat.append(QJsonObject{{"role", "user"}, {"content", "你是软件“芷宁 Angelica+”的智能小助手（你需要自称为“智能小助手”），只能回答医学方面的问题，且以中医为先，对于其他问题，你只能拒绝回答并表达歉意。另外在回答时，无需使用Markdown语法，全部使用正文不加粗字体即可。"}});
    aiConsat.append(QJsonObject{{"role", "user"}, {"content", "如果患者要求开药方，需在末尾强调医药需辨证施治，建议用户如有不适请尽快到正规医院就诊，避免自行用药的风险"}});

    loadMovie = new QMovie(this);
    loadMovie->setFileName(":/new/ai/resources/ai/loading.gif");

    //必须先启动，确保帧加载，否则setScaledContents会提示"Pixmap is a null pixmap"
    loadMovie->start();
    loadMovie->stop();
    ui->load_label->setMovie(loadMovie);
    ui->load_label->setScaledContents(true);
    ui->load_label->setAttribute(Qt::WA_TranslucentBackground , true);
    ui->load_label->setAutoFillBackground(false);
    ui->load_label->setVisible(false);

    //记录区块
    QMovie *mv=new QMovie(this);
    mv->setFileName(":/new/ai/resources/ai/loading.gif");
    ui->loadRec_label->setMovie(mv);
    ui->loadRec_label->movie()->stop();

    ui->rec_tableWidget->setColumnCount(1); // 只需一列（每个 recordItem 占满整行）
    ui->rec_tableWidget->horizontalHeader()->hide(); // 隐藏水平表头
    ui->rec_tableWidget->verticalHeader()->hide();   // 隐藏垂直表头
    ui->rec_tableWidget->setShowGrid(false);         // 隐藏表格线
    ui->rec_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选中
    ui->rec_tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->rec_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->rec_tableWidget->verticalHeader()->setDefaultSectionSize(123);
    ui->rec_tableWidget->setFocusPolicy(Qt::NoFocus);

    //我的就诊人模块
    ui->pat_tableWidget->setColumnCount(1); // 只需一列（每个 recordItem 占满整行）
    ui->pat_tableWidget->horizontalHeader()->hide(); // 隐藏水平表头
    ui->pat_tableWidget->verticalHeader()->hide();   // 隐藏垂直表头
    ui->pat_tableWidget->setShowGrid(false);         // 隐藏表格线
    ui->pat_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选中
    ui->pat_tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->pat_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->pat_tableWidget->verticalHeader()->setDefaultSectionSize(123);
    ui->pat_tableWidget->setFocusPolicy(Qt::NoFocus);
    connect(ui->patRet_btn, &QPushButton::clicked, this, [=]() {ui->loadPat_label->movie()->stop();ui->stackedWidget->setCurrentIndex(0);});

    QMovie *mv2=new QMovie(this);
    mv2->setFileName(":/new/ai/resources/ai/loading.gif");
    ui->loadPat_label->setMovie(mv2);
    ui->loadPat_label->movie()->stop();

    connect(ui->failLoadPat_btn, &QPushButton::clicked, this, [=]() {on_myPat_btn_clicked();});

    connect(ui->addPat_btn, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(7);
        ui->patName_lineEdit->clear();
        ui->patIDType_comboBox->setCurrentIndex(0);
        ui->patID_lineEdit->clear();
        ui->patPhone_lineEdit->clear();
        ui->patCons_label->setText("未测试");
        ui->ModOrDel_label->setText("添加就诊人");
    });

    connect(ui->addPatRet_btn, &QPushButton::clicked, this, [=]() {on_myPat_btn_clicked();});

    connect(ui->test_btn, &QPushButton::clicked, this, [=]() {
        quesMainWindow *ques=new quesMainWindow(this);

        ques->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动销毁

        //子窗口销毁时获取结果
        connect(ques, &quesMainWindow::destroyed, [=]() {
            QString res=ques->getResult();
            //qDebug()<<res;
            if(!res.isEmpty())
                ui->patCons_label->setText(res);
            });
        ques->show();
    });


    //消息窗口
    AngMessageWindow *amw=new AngMessageWindow("");
    //amw->showMsg();


    //皮肤病图鉴区块
    connect(ui->illustRet_btn, &QPushButton::clicked, this, [=]() {on_home_btn_clicked();});
    connect(ui->disConRet_btn, &QPushButton::clicked, this, [=]() {on_skin_btn_clicked();});

    QMovie *mv3=new QMovie(this);
    mv3->setFileName(":/new/ai/resources/ai/loading.gif");
    ui->loadIllust_label->setMovie(mv3);
    ui->loadIllust_label->movie()->stop();

    loadingRecord(ui->scrollArea_3,ui->failLoadIllust_btn,nullptr,ui->loadIllust_label,loadingState::SUCCESS);
    createBtnGroup();

    //新增预约区块
    connect(ui->resRet_btn,&QPushButton::clicked, this, [=]() {on_home_btn_clicked();});

    //打卡区块
    QMovie *mv4=new QMovie(this),*mv5=new QMovie(this);
    mv4->setFileName(":/new/ai/resources/ai/loading.gif");
    mv5->setFileName(":/new/ai/resources/ai/loading.gif");
    ui->staLoad_label->setMovie(mv4);
    ui->staLoad_label2->setMovie(mv5);
    ui->staLoad_label->movie()->stop();
    ui->staLoad_label2->movie()->stop();

    //用户区块
    connect(ui->cancelUserMod_btn,&QPushButton::clicked,[this]{
        this->userPageEditable(false);
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

//主页
void MainWindow::on_home_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    changeStyle(0);

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
    QUrl url(QString("http://localhost:5000/api/assessments/today/%1").arg(USER_ID));

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());
    //qDebug()<<userSettings.value("auth/token", "").toString();
    QNetworkReply *reply=networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject())
            {
                QJsonObject data = jsonDoc.object();
                //qDebug()<<data;
                if(data["itch_assessed"].toInt()==1)
                {
                    ui->itchGrade_label->setText(QString::number(data["itch_value"].toInt()));
                    ui->subItch_btn->setEnabled(false);
                    ui->subItch_btn->setText("今日\n已打卡");
                    sw1->setEnable(false);
                }
                else
                {
                    ui->subItch_btn->setEnabled(true);
                    ui->subItch_btn->setText("提交");
                    sw1->setEnable(true);
                }

                if(data["pain_assessed"].toInt()==1)
                {
                    ui->painGrade_label->setText(QString::number(data["pain_value"].toInt()));
                    ui->subPain_btn->setEnabled(false);
                    ui->subPain_btn->setText("今日\n已打卡");
                    sw2->setEnable(false);
                }
                else
                {
                    ui->subPain_btn->setEnabled(true);
                    ui->subPain_btn->setText("提交");
                    sw2->setEnable(true);
                }
            }
        }
//        else
//        {
//            QMessageBox::warning(this, "错误", "加载用户信息失败");
//        }
        reply->deleteLater(); // 清理
    });
}

//挂号记录
void MainWindow::on_record_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    changeStyle(1);
    ui->allRec_btn->click();
}

//用户界面
void MainWindow::on_user_clicked()
{
    ui->userHead_label->setStyleSheet("width:80px;"
            "height:80px;"
            "border-radius:40px;"
            "background-color: rgb(255, 255, 255);"
            "border:1px solid #f0f0f0;"
            "image:url("+user_head+");");

    userPageEditable(false);

    ui->stackedWidget->setCurrentIndex(5);
    changeStyle(5);

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
    QUrl url(QString("http://localhost:5000/api/user/queryUser/%1").arg(USER_ID));

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply=networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject())
            {
                QJsonObject user = jsonDoc.object();
                //qDebug()<<jsonObj;

                ui->user_label->setText(user["user_name"].toString());
                ui->userID_label2->setText(QString::number(USER_ID));
                QString status=user["status"].toString();
                ui->userState_label2->setText(status);
                if(status=="挂号中")
                {
                    ui->userState_label2->setStyleSheet("font: 12pt \"微软雅黑\";"
                                                        "color: rgb(255, 0, 0);");
                }
                else if(status=="无挂号")
                {
                    ui->userState_label2->setStyleSheet("font: 12pt \"微软雅黑\";"
                                                        "color: rgb(0, 0, 255);");
                }

                int credit=user["credit_level"].toInt();
                ui->credit_label->setToolTip(QString("本月可允许%1次爽约").arg(credit));
                if(credit==2)
                {
                    ui->credit_label->setStyleSheet("QLabel{image:url(:/new/credit/resources/credit/smile.png);}"
                                                    "QToolTip{background-color:rgb(255,255,255);color:#000;}");
                }
                else if(credit==1)
                {
                    ui->credit_label->setStyleSheet("QLabel{image:url(:/new/credit/resources/credit/calm.png);}"
                                                    "QToolTip{background-color:rgb(255,255,255);color:#000;}");
                }
                else
                {
                    ui->credit_label->setStyleSheet("QLabel{image:url(:/new/credit/resources/credit/sad.png);}"
                                                    "QToolTip{background-color:rgb(255,255,255);color:#000;}");
                }
                QString phone=user["phone_number"].toString();
                ui->userTele_label2->setText(!phone.isEmpty()?phone:"未绑定");
                QString email=user["email"].toString();
                ui->userEmail_label2->setText(!email.isEmpty()?email:"未绑定");
                QString wx=user["wechat"].toString();
                ui->userwx_label2->setText(!wx.isEmpty()?wx:"未绑定");
            }
        }
        else
        {
            QMessageBox::warning(this, "错误", "加载用户信息失败");
        }
        reply->deleteLater(); // 清理
    });
}

//ai问答界面
void MainWindow::on_ai_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    changeStyle(2);

    if(firstOpenAi)
    {
        firstOpenAi=false;
        //初始化智能助手对话
        QNChatMessage* messageW = new QNChatMessage(ui->ai_listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->ai_listWidget);
        dealMessage(messageW, item, "您好！我是智能小助手，专注于皮肤健康领域的知识解答。很高兴为您提供帮助，请问您有什么问题需要解决吗？", QNChatMessage::User_AI);
        ui->ai_listWidget->setCurrentRow(ui->ai_listWidget->count()-1);
    }
}


//设置
void MainWindow::on_setting_btn_clicked()
{
//    ui->setHead_label->setStyleSheet("width:80px;"
//            "height:80px;"
//            "border-radius:40px;"
//            "background-color: rgb(255, 255, 255);"
//            "border:1px solid #f0f0f0;"
//            "image:url("+user_head+");");

    ui->stackedWidget->setCurrentIndex(3);
    changeStyle(3);
}

void MainWindow::on_static_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    changeStyle(4);

    loadingRecord(ui->itchWidget,ui->staReload_btn,ui->staNoData_label,ui->staLoad_label,loadingState::LOADING);
    loadingRecord(ui->painWidget,ui->staReload_btn2,ui->staNoData_label2,ui->staLoad_label2,loadingState::LOADING);

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
    QUrl url(QString("http://localhost:5000/api/assessments/weekly_records/%1").arg(USER_ID));

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply=networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject())
            {
                QJsonObject jsonObj = jsonDoc.object();
                //qDebug()<<jsonObj;

                QMap<QDate, int> itchData;
                QMap<QDate, int> painData;

                QJsonArray records = jsonObj["records"].toArray();


                for (const QJsonValue& recordValue : records)
                {
                    QJsonObject record = recordValue.toObject();

                    // 提取日期
                    QString dateStr = record["date"].toString();
                    QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");

                    // 提取 itch_value
                    if (record.contains("itch_value") && !record["itch_value"].isNull()) {
                        int itchValue = record["itch_value"].toInt();
                        itchData.insert(date, itchValue);
                    }

                    // 提取 pain_value
                    if (record.contains("pain_value") && !record["pain_value"].isNull()) {
                        int painValue = record["pain_value"].toInt();
                        painData.insert(date, painValue);
                    }
                }

                if(itchData.size()>0)
                {
                    QChartView *chartView1 = new QChartView(createChart(itchData));
                    chartView1->setRenderHint(QPainter::Antialiasing);

                    QVBoxLayout *vl=new QVBoxLayout();
                    vl->setMargin(0);
                    vl->addWidget(chartView1);
                    ui->itchWidget->setLayout(vl);

                    loadingRecord(ui->itchWidget,ui->staReload_btn,ui->staNoData_label,ui->staLoad_label,loadingState::SUCCESS);
                }
                else
                {
                    loadingRecord(ui->itchWidget,ui->staReload_btn,ui->staNoData_label,ui->staLoad_label,loadingState::NO_RESULT);
                }

                if(painData.size()>0)
                {
                    QChartView *chartView2 = new QChartView(createChart(painData));
                    chartView2->setRenderHint(QPainter::Antialiasing);

                    QVBoxLayout *vl2=new QVBoxLayout();
                    vl2->setMargin(0);
                    vl2->addWidget(chartView2);
                    ui->painWidget->setLayout(vl2);

                    loadingRecord(ui->painWidget,ui->staReload_btn2,ui->staNoData_label2,ui->staLoad_label2,loadingState::SUCCESS);
                }
                else
                {
                    loadingRecord(ui->painWidget,ui->staReload_btn2,ui->staNoData_label2,ui->staLoad_label2,loadingState::NO_RESULT);
                }

//                qDebug() << "Itch Data:";
//                for (auto it = itchData.begin(); it != itchData.end(); ++it)
//                {
//                    qDebug() << "{" << it.key() << "," << it.value() << "}";
//                }

//                qDebug() << "Pain Data:";
//                for (auto it = painData.begin(); it != painData.end(); ++it)
//                {
//                    qDebug() << "{" << it.key() << "," << it.value() << "}";
//                }
            }
        }
        else
        {
            loadingRecord(ui->itchWidget,ui->staReload_btn,ui->staNoData_label,ui->staLoad_label,loadingState::FAIL);
            loadingRecord(ui->painWidget,ui->staReload_btn2,ui->staNoData_label2,ui->staLoad_label2,loadingState::FAIL);
        }
        reply->deleteLater(); // 清理
    });

}

void MainWindow::on_update_btn_clicked()
{
    QMessageBox::warning(this,"提示","暂无更新",QMessageBox::Ok);
}

void MainWindow::on_advice_btn_clicked()
{
    QMessageBox::warning(this,"提示","暂无渠道反馈",QMessageBox::Ok);
}

void MainWindow::on_service_btn_clicked()
{
    QMessageBox::warning(this,"提示","暂无客服",QMessageBox::Ok);
}

void MainWindow::changeStyle(int changeTo)
{
    if(curIndex==changeTo)
        return;

    if(changeTo==5)//转到用户页
    {
        ui->user->setStyleSheet("border:1px solid rgb(0, 171, 149);"+user_style);
        ui->user_frame->setStyleSheet("border:0;"
                    "background-color: qlineargradient(spread:pad, x1:0.9, y1:0, x2:1, y2:0, stop:0.8 rgba(0, 171, 149, 0.1), stop:1 rgba(0, 171, 149, 1));");
    }
    else//转到其他页
    {
        leftBtnGroup[changeTo]->setStyleSheet(btn_actStySheet+btn_style[changeTo][1]+");}");
    }

    if(curIndex==5)//从用户页转到其他页
    {
        ui->user->setStyleSheet("border:1px solid #999999;"+user_style);
        ui->user_frame->setStyleSheet("border:0;"
                    "background-color:rgba(255,255,255,0);");
    }
    else//从非用户页跳到其他非用户页
    {
        leftBtnGroup[curIndex]->setStyleSheet(btn_stySheet+btn_style[curIndex][0]+");}");
    }

    curIndex=changeTo;
}

void MainWindow::on_que_btn_clicked()
{
    AngTableDialog *atDialog=new AngTableDialog(nullptr,"选择就诊人");
    atDialog->setWindowTitle("芷宁 Angelica+ —— 选择就诊人");

    //将AngTableDialog的重新加载信号和网络请求绑定，在调用exec前先按下AngTableDialog的重新加载按钮以实现加载
    connect(atDialog,&AngTableDialog::reload,this,[=]{
        QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
        QUrl url(QString("http://localhost:5000/api/patients/%1/queryPatients").arg(USER_ID));
        QNetworkRequest request(url);
        request.setRawHeader("Authorization",
                           QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

        // 发送GET请求
        QNetworkReply *reply = networkManager->get(request);

        connect(reply, &QNetworkReply::finished,
                    this, [=]() {
            reply->deleteLater();

            // 错误处理
            if (reply->error() != QNetworkReply::NoError)
            {
                qDebug() << "请求错误:" << reply->errorString();
                atDialog->setState(loadingState::FAIL);
                return;
            }

            // 解析 JSON
            QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
            if (jsonDoc.isNull())
            {
                qDebug() << "JSON 解析失败";
                atDialog->setState(loadingState::FAIL);
                return;
            }

            QJsonObject rootObj = jsonDoc.object();

            // 获取患者列表数组
            QJsonArray patientsArray = rootObj["data"].toArray();
            if (patientsArray.isEmpty())
            {
                qDebug() << "无就诊人数据";
                atDialog->setState(loadingState::NO_RESULT);
                return;
            }

            atDialog->setState(loadingState::SUCCESS);

            // 遍历所有就诊人信息
            int row=0;
            QTableWidget *tw=atDialog->getTableWidget();
            for (const QJsonValue &value : patientsArray)
            {
                tw->setRowCount(row+1);

                QJsonObject patient = value.toObject();

                // 提取字段（处理可能的空值）
                int patNum=patient["PATIENT_NUMBER"].toInt();
                QString name = patient["NAME"].toString();
                QString idType = patient["ID_TYPE"].toString();

                ID_TYPE it;

                if(idType=="居民身份证") it=ID_TYPE::DALU;
                else if(idType=="护照") it=ID_TYPE::HUZHAO;
                else if(idType=="港澳居民来往内地通行证") it=ID_TYPE::GANGAO;
                else if(idType=="台湾居民来往大陆通行证") it=ID_TYPE::TAIWAN;

                QString idNum = patient["ID_NUMBER"].toString();
                QString phone = patient["PHONE"].toString();
                QString cons = patient["CONSTITUTION"].isNull()
                                     ? ""
                                     : patient["CONSTITUTION"].toString();

                patientItem *pi1=new patientItem(patNum,name,it,idNum,phone,cons,tw);
                connect(pi1,&patientItem::itemClicked,this,[=](const int &patNum){
                    //qDebug()<<patNum;
                    atDialog->close();

                    //选择就诊人后，窗口关闭，调出判定表
                    quesMainWindow *ques=new quesMainWindow(this);
                    ques->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动销毁

                    //子窗口销毁时获取结果
                    connect(ques, &quesMainWindow::destroyed, [=]() {
                        QString res=ques->getResult();
                        //qDebug()<<res;
                        if(!res.isEmpty())
                        {
                            //修改就诊人体质
                            QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);

                            QJsonObject requestData;
                            requestData["patient_number"] = patNum;
                            requestData["constitution"] = res;

                            // 发送POST请求（URL包含user_id）
                            QUrl url(QString("http://localhost:5000/api/patients/%1/modPatConstitution").arg(USER_ID));
                            QNetworkRequest request(url);
                            request.setRawHeader("Content-Type", "application/json");
                            request.setRawHeader("Authorization",
                                               QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

                            QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());

                            // 处理响应
                            connect(reply, &QNetworkReply::finished, [=]() {
                                if (reply->error() == QNetworkReply::NoError) {
                                    QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
                                    if (response.object()["success"].toBool()) {
                                        QMessageBox::information(this, "成功", "成功添加体质信息");
                                    }
                                } else {
                                    QMessageBox::critical(this, "错误", QString("体质信息添加失败: %1").arg(reply->errorString()));
                                }
                                reply->deleteLater();
                            });
                        }
                    });
                    ques->show();
                });

                pi1->switchToSelectable();
                tw->setCellWidget(row++, 0, pi1);

            }
        });
    });
    atDialog->clickReloadBtn();
    atDialog->exec();
}

void MainWindow::initBtnStyle()
{
    btn_stySheet="QPushButton:hover{"
            "background-color:rgba(0,171,149,0.1);"
            "border-radius:12px;}  "
            "QPushButton{"
            "border:0px;"
            "color:rgb(138,138,138);"
            "background-origin: content;"
            "background-position: top;"
            "background-repeat: no-repeat;"
            "font: 9pt \"微软雅黑\";"
            "height:109px;"
            "width:109px;"
            "padding-top:10px;"
            "padding-bottom:7px;"
            "text-align:bottom;"
            "background-color: rgba(255, 255, 255,0);"
            "background-image:url(";

    btn_actStySheet="QPushButton{"
                    "border:0px;"
                    "border-radius:12px;"
                    "color:rgb(0,171,149);"
                    "background-origin: content;"
                    "background-position: top;"
                    "background-repeat: no-repeat;"
                    "font: 75 9pt \"微软雅黑\";"
                    "font-weight:bold;"
                    "height:109px;"
                    "width:109px;"
                    "padding-top:10px;"
                    "padding-bottom:7px;"
                    "text-align:bottom;"
                    "background-color: qlineargradient(spread:pad, x1:0.9, y1:0, x2:1, y2:0, stop:0.8 rgba(0, 171, 149, 0.1), stop:1 rgba(0, 171, 149, 1));"
                    "background-image:url(";

    recBtn_style="QPushButton{"
            "background-color:rgba(0,171,149,0.7);"
            "color:#fff;"
            "min-width:  120px;"
            "max-width:  120px;"
            "min-height: 40px;"
            "max-height: 40px;"
            "border:1px solid #8a8a8a;"
            "border-bottom:0;"
            "border-top-left-radius:8px;"
            "border-top-right-radius:8px;"
            "font: 10pt \"微软雅黑\";}"
            "QPushButton:hover{"
            "background-color:rgba(0,171,149,0.6);}";

    recBtn_actStyle="QPushButton{"
            "background-color:rgba(0,171,149,1);"
            "color:#fff;"
            "min-width:  120px;"
            "max-width:  120px;"
            "min-height: 50px;"
            "max-height: 50px;"
            "border:1px solid #8a8a8a;"
            "border-bottom:0;"
            "border-top-left-radius:8px;"
            "border-top-right-radius:8px;"
             "   font: 10pt \"微软雅黑\";}";

    btn_style[0][0]=":/new/home/resources/home/home.png";
    btn_style[0][1]=":/new/home/resources/home/home-hover.png";

    btn_style[1][0]=":/new/record/resources/record/record-60.png";
    btn_style[1][1]=":/new/record/resources/record/record-60-hover.png";

    btn_style[2][0]=":/new/ai/resources/ai/messages-60.png";
    btn_style[2][1]=":/new/ai/resources/ai/messages-60-hover.png";

    btn_style[3][0]=":/new/setting/resources/setting/setting-60.png";
    btn_style[3][1]=":/new/setting/resources/setting/setting-60-hover.png";

    btn_style[4][0]=":/new/chart/resources/chart/line-chart-60.png";
    btn_style[4][1]=":/new/chart/resources/chart/line-chart-60-hover.png";
}

void MainWindow::on_agreement_btn_clicked()
{
    AngTxtDialog *dial=new AngTxtDialog(this);

    QFile file(":/new/text/UserAgreement.txt");
    if (!file.open(QIODevice::ReadOnly))
    {
         QMessageBox::question(this,"提示！","打开失败！",QMessageBox::Ok);
         return;
    }
    QByteArray array=file.readAll();
    QString data_in=QString(array);
    dial->setText(data_in);
    dial->setTitle("用户协议");

    // 显示窗口（非模态，允许同时操作主窗口）
    dial->show();
}

void MainWindow::on_policy_btn_clicked()
{
    AngTxtDialog *dial=new AngTxtDialog(this);

    QFile file(":/new/text/PrivacyAgreement.txt");
    if (!file.open(QIODevice::ReadOnly))
    {
         QMessageBox::question(this,"提示！","打开失败！",QMessageBox::Ok);
         return;
    }
    QByteArray array=file.readAll();
    QString data_in=QString(array);
    dial->setText(data_in);
    dial->setTitle("隐私政策");

    // 显示窗口（非模态，允许同时操作主窗口）
    dial->show();
}

void MainWindow::on_aiSend_btn_clicked()
{    
    QString msg = ui->ai_textEdit->toPlainText();

    if(msg=="")
        return;

    QNetworkAccessManager *aiManager = new QNetworkAccessManager(this);

    // 构造请求数据
    QJsonArray consat=QJsonArray(aiConsat);
    consat.append(QJsonObject{{"role", "user"}, {"content",msg}});

    QJsonObject requestData;
    requestData["messages"] = consat;

    // 创建网络请求
    QNetworkRequest request(QUrl(QString("http://localhost:5000/chat/%1").arg(USER_ID)));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply = aiManager->post(request, QJsonDocument(requestData).toJson());

    ui->aiSend_btn->setEnabled(false);
    ui->load_label->setVisible(true);
    loadMovie->start();

    // 处理响应
    connect(reply, &QNetworkReply::finished, [=]() {
        if(reply->error() == QNetworkReply::NoError) {
            QString answer =QString(reply->readAll());

            //用户发出信息
            ui->ai_textEdit->setText("");
            QNChatMessage* messageW = new QNChatMessage(ui->ai_listWidget->parentWidget());
            QListWidgetItem* item = new QListWidgetItem(ui->ai_listWidget);
            dealMessage(messageW, item, msg, QNChatMessage::User_Me);
            ui->ai_listWidget->setCurrentRow(ui->ai_listWidget->count()-1);

            //AI发送信息
            messageW = new QNChatMessage(ui->ai_listWidget->parentWidget());
            item = new QListWidgetItem(ui->ai_listWidget);
            dealMessage(messageW, item, answer, QNChatMessage::User_AI);
            ui->ai_listWidget->setCurrentRow(ui->ai_listWidget->count()-1);

            //用户提问和AI的回答加入对话内容
            aiConsat.append(QJsonObject{{"role", "user"}, {"content",msg}});
            aiConsat.append(QJsonObject{{"role", "system"}, {"content",answer}});

            ui->aiSend_btn->setEnabled(true);
            loadMovie->stop();
            ui->load_label->setVisible(false);

        } else {
            ui->aiSend_btn->setEnabled(true);
            loadMovie->stop();
            ui->load_label->setVisible(false);
            QMessageBox::critical(this, "错误", "请检查网络设置！");
        }
        reply->deleteLater();
    });

}

void MainWindow::dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(ui->ai_listWidget->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, size, type);
    ui->ai_listWidget->setItemWidget(item, messageW);
}

//当窗口尺寸发生变化，自适应控件大小
void MainWindow::resizeEvent(QResizeEvent *event)
{
    //改变对话框大小
    Q_UNUSED(event);
    for(int i = 0; i < ui->ai_listWidget->count(); i++) {
        QNChatMessage* messageW = (QNChatMessage*)ui->ai_listWidget->itemWidget(ui->ai_listWidget->item(i));
        QListWidgetItem* item = ui->ai_listWidget->item(i);

        dealMessage(messageW, item, messageW->text(), messageW->userType());
    }
}

void MainWindow::on_allRec_btn_clicked()
{
    changeRecBtn(0);
    ui->rec_tableWidget->clear();

    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::LOADING);

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
    QUrl url(QString("http://localhost:5000/api/appointments/queryByStatus/%1").arg(USER_ID));

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply=networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                QString status = jsonObj["status"].toString();
                QJsonArray dataArray = jsonObj["data"].toArray();

                // 检查是否有数据
                if (dataArray.isEmpty())
                {
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::NO_RESULT);
                }
                else
                {
                    int row=0;
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::SUCCESS);
                    for (const QJsonValue &value : dataArray)
                    {
                        QJsonObject appo = value.toObject();
                        ui->rec_tableWidget->setRowCount(row+1);
                        QString appointment_id=QString::number(appo["appointment_id"].toInt());
                        QString patient_name=appo["patient_name"].toString();
                        //QString appointment_date=appo["appointment_date"].toString();
                        QString rawDate = appo["appointment_date"].toString();
                        QString timeSlot = appo["time_slot"].toString();

                        //解析并格式化日期 (从 "Fri, 11 Apr 2025 00:00:00 GMT" 到 "2025-04-11")
                        QDateTime dateTime = QDateTime::fromString(rawDate, Qt::RFC2822Date);
                        QString formattedDate = dateTime.date().toString("yyyy-MM-dd");

                        QString appointment_date = QString("%1 %2").arg(formattedDate).arg(timeSlot);

                        QString hospital_name=appo["hospital_name"].toString();
                        QString status=appo["status"].toString();

                        orderStatus os;
                        if(status=="已完成")
                            os=orderStatus::FINISH;
                        else if(status=="待就诊")
                            os=orderStatus::WAITING;
                        else if(status=="已取消")
                            os=orderStatus::CANCEL;
                        else if(status=="已爽约")
                            os=orderStatus::BREAK;

                        recordItem *ri1=new recordItem(appointment_id,patient_name,appointment_date,hospital_name,os,ui->rec_tableWidget);
                        ui->rec_tableWidget->setCellWidget(row++, 0, ri1);
                        connect(ri1, &recordItem::clicked, this, &MainWindow::handleRecordItemClick);
                    }
                }
            }
        } else
        {
            loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::FAIL);
        }
        reply->deleteLater(); // 清理
    });
}

void MainWindow::on_waitRec_btn_clicked()
{
    changeRecBtn(1);
    ui->rec_tableWidget->clear();

    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::LOADING);

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
    QUrl url(QString("http://localhost:5000/api/appointments/queryByStatus/%1").arg(USER_ID));
    url.setQuery(QString("status=%1").arg("待就诊"));

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply=networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                QString status = jsonObj["status"].toString();
                QJsonArray dataArray = jsonObj["data"].toArray();

                // 检查是否有数据
                if (dataArray.isEmpty())
                {
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::NO_RESULT);
                }
                else
                {
                    int row=0;
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::SUCCESS);
                    for (const QJsonValue &value : dataArray)
                    {
                        QJsonObject appo = value.toObject();
                        ui->rec_tableWidget->setRowCount(row+1);
                        QString appointment_id=QString::number(appo["appointment_id"].toInt());
                        QString patient_name=appo["patient_name"].toString();
                        //QString appointment_date=appo["appointment_date"].toString();
                        QString rawDate = appo["appointment_date"].toString();
                        QString timeSlot = appo["time_slot"].toString();

                        //解析并格式化日期 (从 "Fri, 11 Apr 2025 00:00:00 GMT" 到 "2025-04-11")
                        QDateTime dateTime = QDateTime::fromString(rawDate, Qt::RFC2822Date);
                        QString formattedDate = dateTime.date().toString("yyyy-MM-dd");

                        QString appointment_date = QString("%1 %2").arg(formattedDate).arg(timeSlot);

                        QString hospital_name=appo["hospital_name"].toString();

                        recordItem *ri1=new recordItem(appointment_id,patient_name,appointment_date,hospital_name,orderStatus::WAITING,ui->rec_tableWidget);
                        ui->rec_tableWidget->setCellWidget(row++, 0, ri1);
                        connect(ri1, &recordItem::clicked, this, &MainWindow::handleRecordItemClick);
                    }
                }
            }
        } else
        {
            loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::FAIL);
        }
        reply->deleteLater(); // 清理
    });
}

void MainWindow::on_finRec_btn_clicked()
{
    changeRecBtn(2);
    ui->rec_tableWidget->clear();

    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::LOADING);

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
    QUrl url(QString("http://localhost:5000/api/appointments/queryByStatus/%1").arg(USER_ID));
    url.setQuery(QString("status=%1").arg("已完成"));

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply=networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                QString status = jsonObj["status"].toString();
                QJsonArray dataArray = jsonObj["data"].toArray();

                // 检查是否有数据
                if (dataArray.isEmpty())
                {
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::NO_RESULT);
                }
                else
                {
                    int row=0;
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::SUCCESS);
                    for (const QJsonValue &value : dataArray)
                    {
                        QJsonObject appo = value.toObject();
                        ui->rec_tableWidget->setRowCount(row+1);
                        QString appointment_id=QString::number(appo["appointment_id"].toInt());
                        QString patient_name=appo["patient_name"].toString();
                        //QString appointment_date=appo["appointment_date"].toString();
                        QString rawDate = appo["appointment_date"].toString();
                        QString timeSlot = appo["time_slot"].toString();

                        //解析并格式化日期 (从 "Fri, 11 Apr 2025 00:00:00 GMT" 到 "2025-04-11")
                        QDateTime dateTime = QDateTime::fromString(rawDate, Qt::RFC2822Date);
                        QString formattedDate = dateTime.date().toString("yyyy-MM-dd");

                        QString appointment_date = QString("%1 %2").arg(formattedDate).arg(timeSlot);

                        QString hospital_name=appo["hospital_name"].toString();

                        recordItem *ri1=new recordItem(appointment_id,patient_name,appointment_date,hospital_name,orderStatus::FINISH,ui->rec_tableWidget);
                        ui->rec_tableWidget->setCellWidget(row++, 0, ri1);
                        connect(ri1, &recordItem::clicked, this, &MainWindow::handleRecordItemClick);
                    }
                }
            }
        } else
        {
            loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::FAIL);
        }
        reply->deleteLater(); // 清理
    });
}

void MainWindow::on_canRec_btn_clicked()
{
    changeRecBtn(3);
    ui->rec_tableWidget->clear();

    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::LOADING);

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
    QUrl url(QString("http://localhost:5000/api/appointments/queryByStatus/%1").arg(USER_ID));
    url.setQuery(QString("status=%1").arg("已取消"));

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply=networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                QString status = jsonObj["status"].toString();
                QJsonArray dataArray = jsonObj["data"].toArray();

                // 检查是否有数据
                if (dataArray.isEmpty())
                {
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::NO_RESULT);
                }
                else
                {
                    int row=0;
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::SUCCESS);
                    for (const QJsonValue &value : dataArray)
                    {
                        QJsonObject appo = value.toObject();
                        ui->rec_tableWidget->setRowCount(row+1);
                        QString appointment_id=QString::number(appo["appointment_id"].toInt());
                        QString patient_name=appo["patient_name"].toString();
                        //QString appointment_date=appo["appointment_date"].toString();
                        QString rawDate = appo["appointment_date"].toString();
                        QString timeSlot = appo["time_slot"].toString();

                        //解析并格式化日期 (从 "Fri, 11 Apr 2025 00:00:00 GMT" 到 "2025-04-11")
                        QDateTime dateTime = QDateTime::fromString(rawDate, Qt::RFC2822Date);
                        QString formattedDate = dateTime.date().toString("yyyy-MM-dd");

                        QString appointment_date = QString("%1 %2").arg(formattedDate).arg(timeSlot);
                        QString hospital_name=appo["hospital_name"].toString();

                        recordItem *ri1=new recordItem(appointment_id,patient_name,appointment_date,hospital_name,orderStatus::CANCEL,ui->rec_tableWidget);
                        ui->rec_tableWidget->setCellWidget(row++, 0, ri1);
                        connect(ri1, &recordItem::clicked, this, &MainWindow::handleRecordItemClick);
                    }
                }
            }
        } else
        {
            loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::FAIL);
        }
        reply->deleteLater(); // 清理
    });
}

void MainWindow::on_breakRec_btn_clicked()
{
    changeRecBtn(4);
    ui->rec_tableWidget->clear();

    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::LOADING);

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
    QUrl url(QString("http://localhost:5000/api/appointments/queryByStatus/%1").arg(USER_ID));
    url.setQuery(QString("status=%1").arg("已爽约"));

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply=networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                QString status = jsonObj["status"].toString();
                QJsonArray dataArray = jsonObj["data"].toArray();

                // 检查是否有数据
                if (dataArray.isEmpty())
                {
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::NO_RESULT);
                }
                else
                {
                    int row=0;
                    loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::SUCCESS);
                    for (const QJsonValue &value : dataArray)
                    {
                        QJsonObject appo = value.toObject();
                        ui->rec_tableWidget->setRowCount(row+1);
                        QString appointment_id=QString::number(appo["appointment_id"].toInt());
                        QString patient_name=appo["patient_name"].toString();
                        //QString appointment_date=appo["appointment_date"].toString();
                        QString rawDate = appo["appointment_date"].toString();
                        QString timeSlot = appo["time_slot"].toString();

                        //解析并格式化日期 (从 "Fri, 11 Apr 2025 00:00:00 GMT" 到 "2025-04-11")
                        QDateTime dateTime = QDateTime::fromString(rawDate, Qt::RFC2822Date);
                        QString formattedDate = dateTime.date().toString("yyyy-MM-dd");

                        QString appointment_date = QString("%1 %2").arg(formattedDate).arg(timeSlot);
                        QString hospital_name=appo["hospital_name"].toString();

                        recordItem *ri1=new recordItem(appointment_id,patient_name,appointment_date,hospital_name,orderStatus::BREAK,ui->rec_tableWidget);
                        ui->rec_tableWidget->setCellWidget(row++, 0, ri1);
                        connect(ri1, &recordItem::clicked, this, &MainWindow::handleRecordItemClick);
                    }
                }
            }
        } else
        {
            loadingRecord(ui->rec_tableWidget,ui->failLoadRec_btn,ui->noRec_label,ui->loadRec_label,loadingState::FAIL);
        }
        reply->deleteLater(); // 清理
    });
}

void MainWindow::changeRecBtn(int changeTo)
{
    if(curRecList==changeTo)
        return;

    recBtnGroup[curRecList]->setStyleSheet(recBtn_style);
    recBtnGroup[changeTo]->setStyleSheet(recBtn_actStyle);

    curRecList=changeTo;
}

void MainWindow::loadingRecord(QWidget *Widget, QPushButton *reloadBtn, QLabel *noDataLabel, QLabel *loadLabel,const loadingState &res)
{
    if(res==SUCCESS)
    {
        Widget->setHidden(false);
        if(noDataLabel!=nullptr)
            noDataLabel->hide();
        loadLabel->movie()->stop();
        loadLabel->hide();
        reloadBtn->hide();
    }
    else if(res==LOADING)
    {
        Widget->hide();
        if(noDataLabel!=nullptr)
            noDataLabel->hide();
        loadLabel->movie()->start();
        loadLabel->setHidden(false);
        reloadBtn->hide();
    }
    else if(res==FAIL)
    {
        Widget->hide();
        if(noDataLabel!=nullptr)
            noDataLabel->hide();
        loadLabel->movie()->stop();
        loadLabel->hide();
        reloadBtn->setHidden(false);
    }
    else if(res==NO_RESULT)
    {
        Widget->hide();
        if(noDataLabel!=nullptr)
            noDataLabel->setHidden(false);
        loadLabel->movie()->stop();
        loadLabel->hide();
        reloadBtn->hide();
    }
}

void MainWindow::handleRecordItemClick(const QString &data)
{
    //qDebug()<<"点击的订单号："<<data;

    int appoID=data.toInt();

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);
    QUrl url(QString("http://localhost:5000/api/appointments/queryDetails/%1/%2").arg(USER_ID).arg(appoID));

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply=networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject())
            {
                QJsonObject appo = jsonDoc.object();
                //qDebug()<<jsonObj;

                appointmentDetail ad;
                ad.patName=appo["patient_name"].toString();
                ad.idType=appo["id_type"].toString();
                ad.idNum=appo["id_number"].toString();
                if(!appo["constitution"].toString().isEmpty())
                    ad.cons=appo["constitution"].toString();
                else
                    ad.cons="未测试";
                ad.phone=appo["phone"].toString();
                ad.hos=appo["hospital_name"].toString();
                ad.hosPos=appo["hospital_region"].toString()+appo["hospital_address"].toString();
                ad.docName=appo["doctor_name"].toString();
                //ad.appoDate=appo["appointment_date"].toString()+" "+appo["time_slot"].toString();
                QString rawDate = appo["appointment_date"].toString();
                QString timeSlot = appo["time_slot"].toString();

                //解析并格式化日期 (从 "Fri, 11 Apr 2025 00:00:00 GMT" 到 "2025-04-11")
                QDateTime dateTime = QDateTime::fromString(rawDate, Qt::RFC2822Date);
                QString formattedDate = dateTime.date().toString("yyyy-MM-dd");

                ad.appoDate = QString("%1 %2").arg(formattedDate).arg(timeSlot);

                ad.desc=appo["disease_description"].toString();
                ad.price=QString::number(appo["price"].toInt());
                ad.status=appo["status"].toString();
                ad.appoID=appoID;

                ad.createTime = appo["created_at"].toString();

                appointmentWidget *aw=new appointmentWidget();
                aw->addData(ad);
                connect(aw,&appointmentWidget::cancel,this,[=](int id){
                    //qDebug()<<id;

                    // 构造请求 URL
                    QString url2(QString("http://localhost:5000/api/appointments/cancel/%1/%2").arg(USER_ID).arg(id));

                    QNetworkRequest request2(url2);

                    request2.setRawHeader("Content-Type", "application/json");
                    request2.setRawHeader("Authorization",
                                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

                    QNetworkReply *reply2 = networkManager->put(request2, QByteArray());

                    connect(reply2, &QNetworkReply::finished,
                                this, [=]() {
                        if (reply2->error() == QNetworkReply::NoError) {
                            // 读取服务器返回数据
                            QByteArray responseData = reply2->readAll();
                            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

                            if (jsonDoc.isObject()) {
                                QJsonObject jsonObj = jsonDoc.object();

                                // 检查返回的消息
                                if (jsonObj.contains("message"))
                                {
                                    QString message = jsonObj["message"].toString();
                                    QMessageBox::information(this, "取消预约成功", message);
                                    aw->close();
                                    if(curIndex==0)
                                        on_home_btn_clicked();
                                    else
                                        on_allRec_btn_clicked();
                                }
                                else if (jsonObj.contains("error"))
                                {
                                    QString error = jsonObj["error"].toString();
                                    QMessageBox::warning(this, "错误", "预约失败");
                                }
                            }
                        } else {
                            // 处理请求错误
                            QMessageBox::warning(this, "网络错误", reply->errorString());
                        }

                        reply2->deleteLater(); // 清理
                    });
                });

                aw->show();
            }
        }
        else
        {
            QMessageBox::warning(this, "错误", "加载订单信息失败");
        }
        reply->deleteLater(); // 清理
    });
}

//表格刷新
void MainWindow::on_failLoadRec_btn_clicked()
{
    recBtnGroup[curRecList]->click();
}

void MainWindow::on_myPat_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);

    loadingRecord(ui->pat_tableWidget,ui->failLoadPat_btn,ui->noPat_label,ui->loadPat_label,loadingState::LOADING);

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);

    QUrl url(QString("http://localhost:5000/api/patients/%1/queryPatients").arg(USER_ID));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    // 发送GET请求
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        reply->deleteLater();

        // 错误处理
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "请求错误:" << reply->errorString();
            loadingRecord(ui->pat_tableWidget,ui->failLoadPat_btn,ui->noPat_label,ui->loadPat_label,loadingState::FAIL);
            return;
        }

        // 解析 JSON
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        if (jsonDoc.isNull())
        {
            qDebug() << "JSON 解析失败";
            loadingRecord(ui->pat_tableWidget,ui->failLoadPat_btn,ui->noPat_label,ui->loadPat_label,loadingState::FAIL);
            return;
        }

        QJsonObject rootObj = jsonDoc.object();

        // 获取患者列表数组
        QJsonArray patientsArray = rootObj["data"].toArray();
        if (patientsArray.isEmpty())
        {
            qDebug() << "无就诊人数据";
            loadingRecord(ui->pat_tableWidget,ui->failLoadPat_btn,ui->noPat_label,ui->loadPat_label,loadingState::NO_RESULT);
            return;
        }

        loadingRecord(ui->pat_tableWidget,ui->failLoadPat_btn,ui->noPat_label,ui->loadPat_label,loadingState::SUCCESS);

        // 遍历所有就诊人信息
        int row=0;
        for (const QJsonValue &value : patientsArray)
        {
            ui->pat_tableWidget->setRowCount(row+1);

            QJsonObject patient = value.toObject();

            // 提取字段（处理可能的空值）
            int patNum=patient["PATIENT_NUMBER"].toInt();
            QString name = patient["NAME"].toString();
            QString idType = patient["ID_TYPE"].toString();

            ID_TYPE it;

            if(idType=="居民身份证") it=ID_TYPE::DALU;
            else if(idType=="护照") it=ID_TYPE::HUZHAO;
            else if(idType=="港澳居民来往内地通行证") it=ID_TYPE::GANGAO;
            else if(idType=="台湾居民来往大陆通行证") it=ID_TYPE::TAIWAN;

            QString idNum = patient["ID_NUMBER"].toString();
            QString phone = patient["PHONE"].toString();
            QString cons = patient["CONSTITUTION"].isNull()
                                 ? ""
                                 : patient["CONSTITUTION"].toString();

            patientItem *pi1=new patientItem(patNum,name,it,idNum,phone,cons,ui->pat_tableWidget);
            ui->pat_tableWidget->setCellWidget(row++, 0, pi1);

            connect(pi1, &patientItem::delBTNclicked, this, &MainWindow::handlepatientItemDel);

            connect(pi1,&patientItem::modBTNclicked,this,[=](const int &patNum, const QString &name, const int &type,const QString &num, const QString &phone, const QString &cons){
                ui->ModOrDel_label->setText("修改就诊人");

                modPatId=patNum;
                ui->patName_lineEdit->setText(name);
                ui->patIDType_comboBox->setCurrentIndex(type);
                ui->patID_lineEdit->setText(num);
                ui->patPhone_lineEdit->setText(phone);
                ui->patCons_label->setText(cons);
                ui->stackedWidget->setCurrentIndex(7);
            });

//            qDebug() << "姓名:" << name;
//            qDebug() << "证件类型:" << idType;
//            qDebug() << "证件号:" << idNum;
//            qDebug() << "电话:" << phone;
//            qDebug() << "体质:" << cons;
//            qDebug() << "-----------------------";
        }
    });
}

void MainWindow::on_subPat_btn_clicked()
{
    QString name = ui->patName_lineEdit->text().trimmed();
    QString idType = ui->patIDType_comboBox->currentText();
    QString idNum = ui->patID_lineEdit->text().trimmed();
    QString phone = ui->patPhone_lineEdit->text().trimmed();
    QString cons;
    if(ui->patCons_label->text()!="未测试")
        cons = ui->patCons_label->text();

    // 必填字段验证
    if (name.isEmpty() || idNum.isEmpty() || phone.isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写必填字段");
        return;
    }

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);

    QJsonObject requestData;
    requestData["name"] = name;
    requestData["id_type"] = idType;
    requestData["id_number"] = idNum;
    requestData["phone"] = phone;
    if (!cons.isEmpty()) {
        requestData["constitution"] = cons;
    }

    //根据控件标题改变功能
    if(ui->ModOrDel_label->text()=="添加就诊人")
    {
        // 发送POST请求（URL包含user_id）
        QUrl url(QString("http://localhost:5000/api/patients/%1/addPatients").arg(USER_ID));
        QNetworkRequest request(url);
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("Authorization",
                           QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

        QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());

        // 处理响应
        connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
                if (response.object()["success"].toBool()) {
                    QMessageBox::information(this, "成功", "就诊人添加成功");
                    on_myPat_btn_clicked();
                }
            } else {
                QMessageBox::critical(this, "错误", QString("添加失败: %1").arg(reply->errorString()));
            }
            reply->deleteLater();
        });
    }
    else if(ui->ModOrDel_label->text()=="修改就诊人")
    {
        QMessageBox::information(this, "成功", "就诊人信息修改成功");
            return;

        // 发送POST请求（URL包含user_id）
        QUrl url(QString("http://localhost:5000/api/patients/%1/modPatients").arg(USER_ID));
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());

        // 处理响应
        connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
                if (response.object()["success"].toBool()) {
                    QMessageBox::information(this, "成功", "就诊人添加成功");
                    on_myPat_btn_clicked();
                }
            } else {
                QMessageBox::critical(this, "错误", QString("添加失败: %1").arg(reply->errorString()));
            }
            reply->deleteLater();
        });
    }
}

void MainWindow::handlepatientItemDel(const int &patNum)
{
    //for(int i=0;i<4;i++) qDebug()<<data[i];

    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setStyleSheet("font: 11pt \"微软雅黑\";");
    msgBox->setAttribute(Qt::WA_DeleteOnClose); // 对话框关闭时自动释放内存
    msgBox->setWindowTitle("删除确认");
    msgBox->setText(QString("确定删除就诊人信息吗？"));

    msgBox->addButton("确定", QMessageBox::YesRole);
    msgBox->addButton("取消", QMessageBox::NoRole);

    if(msgBox->exec()==1)
        return;

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);

    QJsonObject requestData;
    requestData["patient_number"] = patNum;

    // 发送POST请求（URL包含user_id）
    QUrl url(QString("http://localhost:5000/api/patients/%1/deletePatients").arg(USER_ID));
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());

    // 处理响应
    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
            if (response.object()["success"].toBool()) {
                QMessageBox::information(this, "成功", "就诊人删除成功");
                on_myPat_btn_clicked();
            }
        } else {
            QMessageBox::critical(this, "错误", QString("删除失败: %1").arg(reply->errorString()));
        }
        reply->deleteLater();
    });
}

void MainWindow::on_skin_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
}

void MainWindow::createBtnGroup()
{

    // 创建按钮组并添加按钮
    disBtnGroup = new QButtonGroup(this);
    disBtnGroup->addButton(ui->dis_btn1, 1);
    disBtnGroup->addButton(ui->dis_btn2, 2);
    disBtnGroup->addButton(ui->dis_btn3, 3);
    disBtnGroup->addButton(ui->dis_btn4, 4);
    disBtnGroup->addButton(ui->dis_btn5, 5);

    // 连接按钮组的idClicked信号到槽函数
    connect(disBtnGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(handleDisBtnClicked(QAbstractButton*)));
}

void MainWindow::handleDisBtnClicked(QAbstractButton *clickedButton)
{
    curIndex=1;
    stagesNum=0;
    ui->prevPage_btn->setEnabled(false);
    ui->nextPage_btn->setEnabled(false);

    // 通过按钮指针获取ID
    int id = disBtnGroup->id(clickedButton);
    //qDebug()<<id;
    ui->stackedWidget->setCurrentIndex(9);
    loadingRecord(ui->scrollArea_3,ui->failLoadIllust_btn,nullptr,ui->loadIllust_label,loadingState::LOADING);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString url = QString("http://localhost:5000/api/diseases/%1/%2").arg(USER_ID).arg(id);
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    // 发送GET请求
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);

            if (doc.isNull()) {
                qDebug() << "JSON 解析失败！";
                loadingRecord(ui->scrollArea_3,ui->failLoadIllust_btn,nullptr,ui->loadIllust_label,loadingState::FAIL);
                return;
            }

            QJsonObject responseObj = doc.object();

            if (responseObj.contains("error")) {
                qDebug() << "错误信息:" << responseObj["error"].toString();
                loadingRecord(ui->scrollArea_3,ui->failLoadIllust_btn,nullptr,ui->loadIllust_label,loadingState::FAIL);
            }
            else
            {
                ui->skinDisName_label->setText(disBtnGroup->button(id)->text());

                QJsonArray stages = responseObj["stages"].toArray();

                loadingRecord(ui->scrollArea_3,ui->failLoadIllust_btn,nullptr,ui->loadIllust_label,loadingState::SUCCESS);

                // 创建Swiper实例
                QList<QObject*> childrenList = ui->swiperWidget->children();
                for (QObject *child : childrenList) {
                    child->deleteLater();   // 将删除推迟到事件循环返回后
                }

                swiper = new Swiper(ui->swiperWidget);
                ui->prevPage_btn->setEnabled(false);
                stagesNum=0;

                // 添加控制按钮（可选）
                connect(ui->prevPage_btn, &QPushButton::clicked, swiper, [=]{
                    swiper->toPrev();
                    curIndex--;
                    if(curIndex==1)
                        ui->prevPage_btn->setEnabled(false);

                    if(curIndex<stagesNum)
                        ui->nextPage_btn->setEnabled(true);

                    ui->stageName_label->setText(disStageName[curIndex]);
                    ui->illustCont_label->setText(disDesc[curIndex]);
                });
                connect(ui->nextPage_btn, &QPushButton::clicked, swiper, [=]{
                    swiper->toNext();
                    curIndex++;
                    if(curIndex==stagesNum)
                        ui->nextPage_btn->setEnabled(false);

                    if(curIndex>1)
                        ui->prevPage_btn->setEnabled(true);

                    ui->stageName_label->setText(disStageName[curIndex]);
                    ui->illustCont_label->setText(disDesc[curIndex]);
                });

                // 布局管理
                QVBoxLayout *layout = new QVBoxLayout(ui->swiperWidget);
                layout->addWidget(swiper);

                //ui->swiperWidget->setLayout(layout);

                for (const QJsonValue &stageVal : stages)
                {
                    QJsonObject stage = stageVal.toObject();
                    QString name = stage["name"].toString();
                    QString image = stage["image"].toString();
                    QString desc = stage["description"].toString();

                    //qDebug() << "阶段名称:" << name << "图片路径:" << image<< "描述:" << desc;

                    if (image.isEmpty())
                        break;

                    stagesNum++;

                    disStageName[stagesNum]=name;
                    disDesc[stagesNum]=desc;

                    swiper->addImage(image);

                }
                ui->stageName_label->setText(disStageName[1]);
                ui->illustCont_label->setText(disDesc[1]);

                if(stagesNum==1)
                    ui->nextPage_btn->setEnabled(false);
                else
                    ui->nextPage_btn->setEnabled(true);
                swiper->beginPaint();
            }
        } else {
            qDebug() << "请求失败:" << reply->errorString();
            loadingRecord(ui->scrollArea_3,ui->failLoadIllust_btn,nullptr,ui->loadIllust_label,loadingState::FAIL);
        }
        reply->deleteLater();
        manager->deleteLater();
    });

}

void MainWindow::on_addRes_btn_clicked()
{
    ui->scrollArea_5->verticalScrollBar()->setValue(0);
    clearReserve();
    ui->stackedWidget->setCurrentIndex(10);
}

void MainWindow::on_choPat_btn_clicked()
{
    AngTableDialog *atDialog=new AngTableDialog(nullptr,"选择就诊人");
    atDialog->setWindowTitle("芷宁 Angelica+ —— 选择就诊人");

    //将AngTableDialog的重新加载信号和网络请求绑定，在调用exec前先按下AngTableDialog的重新加载按钮以实现加载
    connect(atDialog,&AngTableDialog::reload,this,[=]{
        QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);

        QUrl url(QString("http://localhost:5000/api/patients/%1/queryPatients").arg(USER_ID));
        QNetworkRequest request(url);
        request.setRawHeader("Authorization",
                           QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

        // 发送GET请求
        QNetworkReply *reply = networkManager->get(request);

        connect(reply, &QNetworkReply::finished,
                    this, [=]() {
            reply->deleteLater();

            // 错误处理
            if (reply->error() != QNetworkReply::NoError)
            {
                qDebug() << "请求错误:" << reply->errorString();
                atDialog->setState(loadingState::FAIL);
                return;
            }

            // 解析 JSON
            QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
            if (jsonDoc.isNull())
            {
                qDebug() << "JSON 解析失败";
                atDialog->setState(loadingState::FAIL);
                return;
            }

            QJsonObject rootObj = jsonDoc.object();

            // 获取患者列表数组
            QJsonArray patientsArray = rootObj["data"].toArray();
            if (patientsArray.isEmpty())
            {
                qDebug() << "无就诊人数据";
                atDialog->setState(loadingState::NO_RESULT);
                return;
            }

            atDialog->setState(loadingState::SUCCESS);

            // 遍历所有就诊人信息
            int row=0;
            QTableWidget *tw=atDialog->getTableWidget();
            for (const QJsonValue &value : patientsArray)
            {
                tw->setRowCount(row+1);

                QJsonObject patient = value.toObject();

                // 提取字段（处理可能的空值）
                int patNum=patient["PATIENT_NUMBER"].toInt();
                QString name = patient["NAME"].toString();
                QString idType = patient["ID_TYPE"].toString();

                ID_TYPE it;
                int type=0;

                if(idType=="居民身份证")
                {
                    it=ID_TYPE::DALU;
                    type=0;
                }
                else if(idType=="护照")
                {
                    it=ID_TYPE::HUZHAO;
                    type=1;
                }
                else if(idType=="港澳居民来往内地通行证")
                {
                    it=ID_TYPE::GANGAO;
                    type=2;
                }
                else if(idType=="台湾居民来往大陆通行证")
                {
                    it=ID_TYPE::TAIWAN;
                    type=3;
                }

                QString idNum = patient["ID_NUMBER"].toString();
                QString phone = patient["PHONE"].toString();
                QString cons = patient["CONSTITUTION"].isNull()
                                     ? ""
                                     : patient["CONSTITUTION"].toString();

                patientItem *pi1=new patientItem(patNum,name,it,idNum,phone,cons,tw);
                connect(pi1,&patientItem::itemClicked,this,[=](const int &patNum)
                {
                    atDialog->close();
                    ui->resPatName_lineEdit->setText(name);
                    ui->resPatIDType_comboBox->setCurrentIndex(type);
                    ui->resPatID_lineEdit->setText(idNum);
                    ui->resPatPhone_lineEdit->setText(phone);
                    if(!cons.isEmpty())
                        ui->resPatCons_label->setText(cons);
                    else
                        ui->resPatCons_label->setText("未测试");
                });

                pi1->switchToSelectable();
                tw->setCellWidget(row++, 0, pi1);
            }
        });
    });
    atDialog->clickReloadBtn();
    atDialog->exec();
}

void MainWindow::on_choHos_btn_clicked()
{
    AngTableDialog *atDialog=new AngTableDialog(nullptr,"选择就诊医院");
    atDialog->setWindowTitle("芷宁 Angelica+ —— 选择就诊医院");

    QString region=ui->resReg_comboBox->currentText();

    connect(atDialog,&AngTableDialog::reload,this,[=]{
        QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);

        QUrl url(QString("http://localhost:5000/api/hospitals/%1/%2/queryByCity").arg(USER_ID).arg(region));
        QNetworkRequest request(url);
        request.setRawHeader("Authorization",
                           QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

        // 发送GET请求
        QNetworkReply *reply = networkManager->get(request);

        connect(reply, &QNetworkReply::finished,
                    this, [=]() {
            reply->deleteLater();

            // 错误处理
            if (reply->error() != QNetworkReply::NoError)
            {
                qDebug() << "请求错误:" << reply->errorString();
                atDialog->setState(loadingState::FAIL);
                return;
            }

            // 解析 JSON
            QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
            if (jsonDoc.isNull())
            {
                qDebug() << "JSON 解析失败";
                atDialog->setState(loadingState::FAIL);
                return;
            }

            QJsonObject rootObj = jsonDoc.object();

            // 获取就诊医院列表数组
            QJsonArray hospitalArray = rootObj["data"].toArray();
            if (hospitalArray.isEmpty())
            {
                qDebug() << "无就诊医院数据";
                atDialog->setState(loadingState::NO_RESULT);
                return;
            }
            //qDebug()<<hospitalArray;

            atDialog->setState(loadingState::SUCCESS);

            // 遍历所有医院信息
            int row=0;
            QTableWidget *tw=atDialog->getTableWidget();
            for (const QJsonValue &value : hospitalArray)
            {
                tw->setRowCount(row+1);

                QJsonObject hospital = value.toObject();

                // 提取字段（处理可能的空值）
                int hosId = hospital["hospital_id"].toInt();
                QString hosName = hospital["name"].toString();
                QString hosLevel = hospital["hospital_level"].toString();
                QString address = hospital["address"].toString();

                QString path = hospital["picture"].toString();

                hospitalItem *hi = new hospitalItem(hosId,hosName,region,address,hosLevel,path);
                connect(hi,&hospitalItem::clicked,this,[=](const int &hosId)
                {
                    ui->resHos_lineEdit->setText(hosName);
                    hospitalId=hosId;
                    atDialog->close();

                });
                tw->setCellWidget(row++, 0, hi);

            }
        });
    });

    atDialog->clickReloadBtn();
    atDialog->exec();
}

void MainWindow::on_choDoc_btn_clicked()
{
    if(ui->resHos_lineEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"提示","请先选择就诊医院",QMessageBox::Ok);
        return;
    }

    doctorWidget *dw=new doctorWidget();
    dw->show();

    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);

    QUrl url(QString("http://localhost:5000/api/doctors/%1/%2/queryByHospitalId").arg(USER_ID).arg(hospitalId));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    // 发送GET请求
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        reply->deleteLater();

        // 错误处理
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "请求错误:" << reply->errorString();
            dw->setState(loadingState::FAIL);
            return;
        }

        // 解析 JSON
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        if (jsonDoc.isNull())
        {
            qDebug() << "JSON 解析失败";
            dw->setState(loadingState::FAIL);
            return;
        }

        QJsonObject rootObj = jsonDoc.object();

        // 获取就诊医院列表数组
        QJsonArray hospitalArray = rootObj["data"].toArray();
        if (hospitalArray.isEmpty())
        {
            qDebug() << "无就诊医院数据";
            dw->setState(loadingState::NO_RESULT);
            return;
        }
        //qDebug()<<hospitalArray;

        dw->setState(loadingState::SUCCESS);

        // 遍历所有医院信息
        for (const QJsonValue &value : hospitalArray)
        {
            QJsonObject doctors = value.toObject();

            doctor doc;

            // 提取字段
            doc.id = doctors["doctor_id"].toInt();
            doc.name = doctors["name"].toString();
            doc.title = doctors["title"].toString();
            doc.department = doctors["department"].toString();
            doc.intro = doctors["introduction"].toString();
            doc.exp = doctors["expertise"].toString();
            doc.price = doctors["price"].toInt();
            doc.path = doctors["picture"].toString();
            doc.hospital=ui->resHos_lineEdit->text();

            doctorItem *di = new doctorItem(doc);
            connect(di,&doctorItem::clicked,dw,[=](const doctor &doc){
                dw->handleItemClicked(doc);
            });

            dw->addItem(di);
        }

        connect(dw,&doctorWidget::submitDoctor,this,[=](int id,QString name,QString price){
            doctorId=id;
            ui->docName_lineEdit->setText(name);
            ui->resPrice_label->setText(price);
            QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);

            QUrl url(QString("http://localhost:5000/api/doctor_schedule/%1/%2/schedule").arg(USER_ID).arg(doctorId));
            QNetworkRequest request(url);
            request.setRawHeader("Authorization",
                               QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

            // 发送GET请求
            QNetworkReply *reply = networkManager->get(request);

            connect(reply, &QNetworkReply::finished,
                        this, [=]() {
                reply->deleteLater();

                // 错误处理
                if (reply->error() != QNetworkReply::NoError)
                {
                    qDebug() << "医师排班日期请求错误:" << reply->errorString();
                    return;
                }

                // 解析 JSON
                QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
                if (jsonDoc.isNull())
                {
                    qDebug() << "医师排班日期JSON 解析失败";
                    return;
                }
                dateToScheID.clear();

                QJsonObject jsonObj = jsonDoc.object();

                QJsonArray scheduleArray = jsonObj["schedule"].toArray();

                ui->resTime_comboBox->clear();

                for (const QJsonValue &value : scheduleArray)
                {
                    QJsonObject scheduleObj = value.toObject();

                    //获取 date 和 time_slot
                    int scheID= scheduleObj["schedule_id"].toInt();
                    QString rawDate = scheduleObj["date"].toString();
                    QString timeSlot = scheduleObj["time_slot"].toString();

                    //解析并格式化日期 (从 "Fri, 11 Apr 2025 00:00:00 GMT" 到 "2025-04-11")
                    QDateTime dateTime = QDateTime::fromString(rawDate, Qt::RFC2822Date);
                    QString formattedDate = dateTime.date().toString("yyyy-MM-dd");

                    QString combined = QString("%1 %2").arg(formattedDate).arg(timeSlot);

                    dateToScheID[combined]=scheID;

                    ui->resTime_comboBox->addItem(combined); //添加到QComboBox
                }
            });

        });
    });
}

void MainWindow::clearReserve()
{
    ui->resPatName_lineEdit->clear();
    ui->resPatIDType_comboBox->setCurrentIndex(0);
    ui->resPatID_lineEdit->clear();
    ui->resPatPhone_lineEdit->clear();
    ui->resPatCons_label->setText("未测试");
    ui->resReg_comboBox->setCurrentIndex(0);
    ui->resHos_lineEdit->clear();
    ui->docName_lineEdit->clear();
    ui->resTime_comboBox->clear();
    ui->resDesc_textEdit->clear();
    ui->resPrice_label->setText("￥0");
}

void MainWindow::on_subRes_btn_clicked()
{
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

    // 整理数据为 JSON
    QJsonObject json;
    json["hospital_id"] = hospitalId;
    json["doctor_id"] = doctorId;
    json["price"] = (ui->resPrice_label->text().mid(1)).toInt();
    json["appointment_date"] = ui->resTime_comboBox->currentText().mid(0,10);
    json["time_slot"] = ui->resTime_comboBox->currentText().mid(11);
    json["disease_description"]=ui->resDesc_textEdit->toPlainText();
    json["patient_name"] = ui->resPatName_lineEdit->text();
    json["id_type"]=ui->resPatIDType_comboBox->currentText();
    json["id_number"]=ui->resPatID_lineEdit->text();
    json["phone"] = ui->resPatPhone_lineEdit->text();
    if(ui->resPatCons_label->text()!="未测试")
        json["constitution"]=ui->resPatCons_label->text();

    json["schedule_id"]=dateToScheID[ui->resTime_comboBox->currentText()];
    //qDebug()<<json;
    // 创建请求
    QUrl url(QString("http://localhost:5000/api/appointments/%1/addAppointment").arg(USER_ID));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());
    //qDebug()<<QString("Bearer %1").arg(userSettings.value("auth/token", "").toString());
    // 发送 POST 请求
    QByteArray jsonData = QJsonDocument(json).toJson();

    QNetworkReply *reply=networkManager->post(request,jsonData);
    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject())
            {
                QJsonObject jsonObj = jsonDoc.object();
                //qDebug()<<jsonObj;
                if(jsonObj["message"].toString().contains("no credit"))
                {
                    QMessageBox::information(this, "提示", "由于账号信用过低，您已被禁止预约挂号！");
                }
                else
                {
                    QMessageBox::information(this, "提示", "预约成功");
                    ui->resRet_btn->click();
                    dateToScheID.clear();
                }

            }
        }
        else
        {
            // 处理错误响应
            if(reply->errorString().contains("BAD REQUEST"))
                QMessageBox::warning(this, "提示", "该医师预约余量不足");
            else
                QMessageBox::warning(this, "错误", reply->errorString());
        }
        reply->deleteLater();
    });
}

void MainWindow::setUserID(const int &id)
{
    USER_ID=id;
    on_home_btn_clicked();
}

void MainWindow::on_exitLogin_btn_clicked()
{
//    login *l=new login();
//    l->show();
//    this->close();
    emit logout();
}

void MainWindow::setToken(const QString &token)
{
    userSettings.setValue("auth/token", token);
    userSettings.setValue("auth/user_id", USER_ID);
    userSettings.sync();
}

void MainWindow::on_subItch_btn_clicked()
{
    if(ui->itchGrade_label->text().isEmpty())
    {
        QMessageBox::warning(this, "提示", "您未进行评分！");
        return;
    }

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

    // 整理数据为 JSON
    QJsonObject json;
    json["itch_value"] = ui->itchGrade_label->text().toInt();

    // 创建请求
    QUrl url(QString("http://localhost:5000/api/assessments/addItch/%1").arg(USER_ID));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    // 发送 POST 请求
    QByteArray jsonData = QJsonDocument(json).toJson();

    QNetworkReply *reply=networkManager->post(request,jsonData);
    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject())
            {
                QJsonObject jsonObj = jsonDoc.object();
                if(!jsonObj["message"].toString().isEmpty())
                {
                    ui->subItch_btn->setDisabled(true);
                    ui->subItch_btn->setText("今日\n已打卡");
                    sw1->setEnable(false);
                }
                else
                {
                    QMessageBox::warning(this, "错误","服务器错误");
                }

            }
        }
        else
        {
            // 处理错误响应
            QMessageBox::warning(this, "错误", reply->errorString());
        }

        reply->deleteLater();
    });
}

void MainWindow::on_subPain_btn_clicked()
{
    if(ui->painGrade_label->text().isEmpty())
    {
        QMessageBox::warning(this, "提示", "您未进行评分！");
        return;
    }

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

    // 整理数据为 JSON
    QJsonObject json;
    json["pain_value"] = ui->painGrade_label->text().toInt();

    // 创建请求
    QUrl url(QString("http://localhost:5000/api/assessments/addPain/%1").arg(USER_ID));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    // 发送 POST 请求
    QByteArray jsonData = QJsonDocument(json).toJson();

    QNetworkReply *reply=networkManager->post(request,jsonData);
    connect(reply, &QNetworkReply::finished,
                this, [=]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            // 处理成功响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject())
            {
                QJsonObject jsonObj = jsonDoc.object();
                if(!jsonObj["message"].toString().isEmpty())
                {
                    ui->subPain_btn->setDisabled(true);
                    ui->subPain_btn->setText("今日\n已打卡");
                    sw2->setEnable(false);
                }
                else
                {
                    QMessageBox::warning(this, "错误","服务器错误");
                }

            }
        }
        else
        {
            // 处理错误响应
            QMessageBox::warning(this, "错误", reply->errorString());
        }

        reply->deleteLater();
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();
    emit closeWindow();
}

QChart* MainWindow::createChart(const QMap<QDate, int>& data)
{
    //确定日期范围（包含所有日期和没有数据的日期）
    if (data.isEmpty())
    {
        return new QChart();
    }

    QDate minDate = data.firstKey();
    QDate maxDate = data.firstKey();

    for (auto it = data.cbegin(); it != data.cend(); ++it) {
        if (it.key() < minDate) minDate = it.key();
        if (it.key() > maxDate) maxDate = it.key();
    }

    QDate startDate = minDate;
    QDate endDate = maxDate;

    //生成完整日期列表
    QList<QDate> fullDateRange;
    for (QDate date = startDate; date <= endDate; date = date.addDays(1))
    {
        fullDateRange.append(date);
    }

    QLineSeries* lineSeries = new QLineSeries();
    QScatterSeries* scatterSeries = new QScatterSeries();

    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterSeries->setMarkerSize(10);

    scatterSeries->setColor(QColor(0,171,149));

    for (auto it = data.cbegin(); it != data.cend(); ++it)
    {
        QDate date = it.key();
        int value = it.value();

        qreal x = date.toJulianDay(); //横轴用Julian Day
        QPointF point(x, value);
        lineSeries->append(point);
        scatterSeries->append(point);
    }

    QFont font("Microsoft YaHei", 8);
    QFont font2("Microsoft YaHei", 10);
    font2.setBold(true);

    //创建图表
    QChart* chart = new QChart();
    chart->addSeries(lineSeries);
    chart->addSeries(scatterSeries);
    chart->legend()->hide();

    //设置纵轴只显示0、6、10
    QValueAxis* yAxis = new QValueAxis();
    yAxis->setRange(0, 10);
    yAxis->setTickCount(3); // 0、6、10
    yAxis->setLabelFormat("%d");
    yAxis->setTitleText("评估分数");
    yAxis->setTitleFont(font2);
    yAxis->setLabelsFont(font);
    chart->addAxis(yAxis, Qt::AlignLeft);
    lineSeries->attachAxis(yAxis);
    scatterSeries->attachAxis(yAxis);

    //设置横轴
    QCategoryAxis* xAxis = new QCategoryAxis();
    xAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    xAxis->setTitleText("日期");
    xAxis->setTitleFont(font2);
    xAxis->setLabelsFont(font);
    // 添加完整日期标签
    for (const QDate& date : fullDateRange)
    {
        QString label = date.toString("MM-dd");
        qreal x = date.toJulianDay();
        //只在有数据的日期点连接标签，避免重复
        xAxis->append(label, x);
    }
    //设置范围稍大一点，避免超出
    qreal minX = startDate.toJulianDay() - 0.5;
    qreal maxX = endDate.toJulianDay() + 0.5;
    xAxis->setRange(minX, maxX);
    chart->addAxis(xAxis, Qt::AlignBottom);
    lineSeries->attachAxis(xAxis);
    scatterSeries->attachAxis(xAxis);

    connect(scatterSeries, &QScatterSeries::hovered, [](const QPointF& point, bool state){
        if (state)
        {
            QString tooltipText = QString("日期：%1\n分数：%2")
                .arg(QDate::fromJulianDay(qRound(point.x())).toString("MM-dd"))
                .arg(point.y());
            QToolTip::showText(QCursor::pos(), tooltipText);
        }
        else
        {
            QToolTip::hideText();
        }
    });

    //去掉网格线
    yAxis->setGridLineVisible(false); // 隐藏纵轴网格线
    xAxis->setGridLineVisible(false); // 隐藏横轴网格线

    return chart;
}

void MainWindow::userPageEditable(const bool &f)
{
    ui->userPhoneEdit->setHidden(!f);
    ui->userEmailEdit->setHidden(!f);
    ui->userWechatEdit->setHidden(!f);
    ui->userPageBtnGroup->setHidden(!f);
    ui->userPageBtnGroup->setHidden(!f);

    if(f)
    {
        if(ui->userTele_label2->text()!="未绑定")
            ui->userPhoneEdit->setText(ui->userTele_label2->text());

        if(ui->userEmail_label2->text()!="未绑定")
            ui->userEmailEdit->setText(ui->userEmail_label2->text());

        if(ui->userwx_label2->text()!="未绑定")
            ui->userWechatEdit->setText(ui->userwx_label2->text());
    }

    ui->userTele_label2->setHidden(f);
    ui->userEmail_label2->setHidden(f);
    ui->userwx_label2->setHidden(f);
}

void MainWindow::on_userMod_btn_clicked()
{
    userPageEditable(true);
}

void MainWindow::on_subUserMod_btn_clicked()
{
    QNetworkAccessManager *networkManager=new QNetworkAccessManager(this);

    QJsonObject requestData;
    requestData["phone"] = ui->userPhoneEdit->text();
    requestData["email"] = ui->userEmailEdit->text();
    requestData["wechat"] = ui->userWechatEdit->text();

    // 发送POST请求（URL包含user_id）
    QUrl url(QString("http://localhost:5000/api/user/%1/modInfo").arg(USER_ID));
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(userSettings.value("auth/token", "").toString()).toUtf8());

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());

    // 处理响应
    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
            if (response.object()["success"].toBool())
                ui->user->click();
        }
        else
        {
            QMessageBox::critical(this, "错误", QString("修改用户信息失败: %1").arg(reply->errorString()));
        }
        reply->deleteLater();
    });
}

void MainWindow::on_disConRet_btn_clicked()
{
    QList<QObject*> childrenList = ui->swiperWidget->children();
    for (QObject *child : childrenList) {
        child->deleteLater();   // 将删除推迟到事件循环返回后
    }
}
