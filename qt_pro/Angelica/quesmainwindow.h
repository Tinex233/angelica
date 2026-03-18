#ifndef QUESMAINWINDOW_H
#define QUESMAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDebug>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <vector>

using namespace std;

namespace Ui {
class quesMainWindow;
}

class quesMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit quesMainWindow(QWidget *parent = nullptr);
    ~quesMainWindow();

    QString getResult();

private slots:
    //关闭按钮
    void on_close_btn_clicked();

    //上一页按钮
    void on_pre_btn_clicked();

    //下一页按钮
    void on_next_btn_clicked();

    //提交按钮
    void on_sub_btn_clicked();

private:
    Ui::quesMainWindow *ui;

    //各表格的按钮组
    vector<QButtonGroup *> btnGroup[9];

    //表格当前页
    int curPage=0;

    //表格总页数
    int numOfPage;

    //各表格对应体质
    QString cons[9];

    //测试结果
    QString res;

    //设置表格样式
    void setTabWidStyle(QTableWidget *tw,int row,int col, int index);

    //设置页码标签
    void setPageLabel(int cur,int sum);

};

#endif // QUESMAINWINDOW_H
