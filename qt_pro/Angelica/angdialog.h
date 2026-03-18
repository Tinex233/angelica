//该文件提供多个功能性窗口类

#ifndef ANGDIALOG_H
#define ANGDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QTextEdit>
#include <QLayout>
#include <QLabel>
#include <QTableWidget>
#include <QMovie>
#include <QDebug>
#include <QPushButton>
#include <QHeaderView>
#include <QMainWindow>
#include <QTimer>
#include <QDesktopWidget>
#include <QApplication>
#include <QPoint>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

//加载状态：成功，失败，正在加载，无结果
enum loadingState{SUCCESS,FAIL,LOADING,NO_RESULT};

//提供一个纯文本窗口
class AngTxtDialog
{
public:
    AngTxtDialog(QWidget *parent = nullptr);

    //设置窗口名
    void setTitle(const QString &str);

    //设置文本
    void setText(const QString &text);

    //显示
    void show();

    ~AngTxtDialog();

private:
    QDialog *textWindow;

    QTextEdit *textEdit;
};

//提供数据表格
class AngTableDialog : public QDialog {
    Q_OBJECT
public:
    explicit AngTableDialog(QWidget *parent = nullptr ,const QString &labelTitle="");

    void setState(const loadingState &state);

    void clickReloadBtn();

    QTableWidget *getTableWidget();

    ~AngTableDialog();

signals:
    //重新加载信号
    void reload();

private:
    QTableWidget *tw;

    QLabel *titleLabel;

    QLabel *loading;

    QPushButton *failLoading;

    QLabel *noData;

};

//提供右下角信息提示窗口
class AngMessageWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AngMessageWindow(const QString &message, QWidget *parent = nullptr);
    ~AngMessageWindow();

    void showMsg();

private slots:
    void onMove();

    void onStay();

    void onClose();

private:
    QTimer *showTimer;

    QTimer *stayTimer;

    QTimer *closeTimer;

    QPoint point;

    int m_nDesktopHeight;

    float transparent=1.0;

};

#endif // ANGDIALOG_H
