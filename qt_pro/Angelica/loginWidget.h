#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>

#include "mainwindow.h"
#include "regiswidget.h"

namespace Ui {
class loginWidget;
}

class loginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit loginWidget(QWidget *parent = nullptr);
    ~loginWidget();

private slots:
    void on_close_btn_clicked();

    void on_login_btn_clicked();

    void on_register_btn_clicked();

    void on_min_btn_clicked();

private:
    Ui::loginWidget *ui;

    MainWindow *w;

    regisWidget *r;

    bool isDragging; //是否正在拖动

    QPoint dragPosition; //拖动时鼠标位置和窗口左上角位置的差值

    QNetworkAccessManager *m_manager;

    QRect draggableArea;//鼠标可拖动范围

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void handleLogoutResponse(QNetworkReply *reply);//处理登出请求

    void clearLocalAuthData();//清除本地token

};

#endif // LOGIN_H
