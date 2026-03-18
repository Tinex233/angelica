#ifndef APPOINTMENTWIDGET_H
#define APPOINTMENTWIDGET_H

#include <QWidget>

namespace Ui {
class appointmentWidget;
}

struct appointmentDetail
{
    int appoID;

    QString patName;

    QString idType;

    QString idNum;

    QString phone;

    QString cons;

    QString hos;

    QString hosPos;

    QString docName;

    QString appoDate;

    QString desc;

    QString price;

    QString status;

    QString createTime;

};

class appointmentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit appointmentWidget(QWidget *parent = nullptr);
    ~appointmentWidget();

    void addData(const appointmentDetail &a);

signals:
    void cancel(const int appoID);

private:
    Ui::appointmentWidget *ui;

    int id;
};

#endif // APPOINTMENTWIDGET_H
