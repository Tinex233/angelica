#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QMouseEvent>

namespace Ui {
class regisWidget;
}

class regisWidget : public QWidget
{
    Q_OBJECT

public:
    explicit regisWidget(QWidget *parent = nullptr);
    ~regisWidget();

signals:
    //返回登录界面
    void returnToLogin();

private slots:


    void on_regis_btn_clicked();

    void on_close_btn_clicked();

    void on_min_btn_clicked();

private:
    Ui::regisWidget *ui;

    bool isDragging; //是否正在拖动

    QPoint dragPosition; //拖动时鼠标位置和窗口左上角位置的差值

    QRect draggableArea;//鼠标可拖动范围

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

};

#endif // REGISTERWIDGET_H
