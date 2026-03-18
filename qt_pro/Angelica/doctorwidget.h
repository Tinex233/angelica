#ifndef DOCTORWIDGET_H
#define DOCTORWIDGET_H

#include <QWidget>

#include "angitem.h"
#include "angdialog.h"

namespace Ui {
class doctorWidget;
}

class doctorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit doctorWidget(QWidget *parent = nullptr);
    ~doctorWidget();

    //添加表格项
    void addItem(doctorItem *doctor);

    void setState(const loadingState &state);

signals:
    //提交就诊医生
    void submitDoctor(int id, QString docName, QString price);

public slots:

    //处理医生项的点击
    void handleItemClicked(const doctor &doc);

private slots:


private:
    Ui::doctorWidget *ui;

    int curDoctor;
};

#endif // DOCTORWIDGET_H
