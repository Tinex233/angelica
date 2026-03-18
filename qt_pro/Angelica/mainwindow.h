#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QMessageBox>
#include <QTextEdit>
#include <QFile>
#include <QDateTime>
#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QMovie>
#include <QWidget>
#include <QListWidgetItem>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QMouseEvent>
#include <QButtonGroup>
#include <QScrollBar>
#include <QLocale>
#include <QSettings>
#include <QtCharts/QtCharts>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QDateTimeAxis>
#include <QChartView>
#include <map>

#include "quesmainwindow.h"
#include "angdialog.h"
#include "qnchatmessage.h"
#include "angitem.h"
#include "swiper.h"
#include "doctorwidget.h"
#include "appointmentwidget.h"
#include "scalewidget.h"

using namespace QtCharts;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //对话框处理：信息处理和更改大小
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QNChatMessage::User_Type type);

    //登录后设置用户ID
    void setUserID(const int &id);

    void resizeEvent(QResizeEvent *event);

    //设置用户token
    void setToken(const QString &token);

signals:

    //登出信号
    void logout();

    //关闭窗口
    void closeWindow();

private slots:
    void on_user_clicked();

    void on_setting_btn_clicked();

    void on_home_btn_clicked();

    void on_update_btn_clicked();

    void on_advice_btn_clicked();

    void on_service_btn_clicked();

    void on_que_btn_clicked();

    void on_agreement_btn_clicked();

    void on_policy_btn_clicked();

    void on_ai_btn_clicked();

    void on_aiSend_btn_clicked();

    void on_record_btn_clicked();

    void on_allRec_btn_clicked();

    void on_waitRec_btn_clicked();

    void on_finRec_btn_clicked();

    void on_canRec_btn_clicked();

    void on_failLoadRec_btn_clicked();

    void on_myPat_btn_clicked();

    void on_subPat_btn_clicked();

    void on_skin_btn_clicked();

    //处理recordItem的点击
    void handleRecordItemClick(const QString &data);

    //处理patientItem的删除
    void handlepatientItemDel(const int &patNum);

    //处理图鉴处按钮的点击
    void handleDisBtnClicked(QAbstractButton *clickedButton);

    void on_addRes_btn_clicked();

    void on_choPat_btn_clicked();

    void on_choHos_btn_clicked();

    void on_choDoc_btn_clicked();

    void on_subRes_btn_clicked();

    void on_breakRec_btn_clicked();

    void on_exitLogin_btn_clicked();

    void on_subItch_btn_clicked();

    void on_subPain_btn_clicked();

    void on_static_btn_clicked();

    void on_userMod_btn_clicked();

    void on_subUserMod_btn_clicked();

    void on_disConRet_btn_clicked();

private:
    Ui::MainWindow *ui;

    //是否开启消息提醒
    bool remindMsg;

    //首次打开智能问答
    bool firstOpenAi;

    //用户账号
    int USER_ID;

    //当前页号
    int curIndex=3;

    //预约记录当前的表
    int curRecList=0;

    //皮肤病当前阶段和总阶段数
    int curStageIndex=1;
    int stagesNum;

    //皮肤病阶段名和描述
    QString disStageName[6];
    QString disDesc[6];

    //侧边栏按钮样式，包含正常状态和选中状态
    QString btn_style[5][2];

    //预约记录的按钮组
    QPushButton *recBtnGroup[5];

    //用户按钮样式
    QString user_style;
    QString user_head;

    QString btn_stySheet;
    QString btn_actStySheet;

    //预约记录页按钮样式
    QString recBtn_style;
    QString recBtn_actStyle;

    //左侧控件组
    QPushButton *leftBtnGroup[5];

    //所有对话，用于保留AI上下文
    QJsonArray aiConsat;

    //问答界面加载图标
    QMovie *loadMovie;

    //图鉴按钮组
    QButtonGroup *disBtnGroup;

    //新增预约时，选择的医院id
    int hospitalId;

    //选择的医生id
    int doctorId;

    //需修改的就诊人id
    int modPatId;

    //建立schedule_id与日期的映射
    map<QString,int> dateToScheID;

    //本地设置
    QSettings userSettings;

    //两个评估条
    scaleWidget *sw1,*sw2;

    //轮播图
    Swiper *swiper=NULL;

    //转换页面时的样式更改
    void changeStyle(int changeTo);

    //初始化按钮样式
    void initBtnStyle();

    //预约记录点击按钮时的样式更改
    void changeRecBtn(int changeTo);

    //表格点击按钮时返回单号
    void getClickedRow(int row);

    //加载记录表格时的样式
    void loadingRecord(QWidget *Widget, QPushButton *reloadBtn, QLabel *noDataLabel, QLabel *loadLabel,const loadingState &res);

    //皮肤病图鉴：创建按钮组
    void createBtnGroup();

    //清空预约内容
    void clearReserve();

    //重写关闭事件，将销毁事件交给父级
    void closeEvent(QCloseEvent *event) override;

    //绘制折线图
    QChart* createChart(const QMap<QDate, int>& data);

    //用户界面切换修改/显示
    void userPageEditable(const bool &f);

};


#endif // MAINWINDOW_H
