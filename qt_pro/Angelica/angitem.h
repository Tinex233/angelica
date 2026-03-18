//该文件主要包含各表项的样式类

#ifndef ANGITEM_H
#define ANGITEM_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QPushButton>
#include <QMessageBox>


//记录表项类

//订单状态：已完成，待就诊，已取消
enum orderStatus {FINISH, WAITING, CANCEL, BREAK};

class recordItem : public QWidget {
    Q_OBJECT
public:
    explicit recordItem(const QString &i,const QString &p,const QString &t,const QString &l,const orderStatus &s,QWidget *parent = nullptr);

signals:
    void clicked(const QString &data); // 点击信号（携带字符串）

protected:
    void mousePressEvent(QMouseEvent *event) override; // 鼠标点击事件

private:
    QLabel *id; //订单号

    QLabel *patient;//就诊人

    QLabel *time;//就诊时间

    QLabel *location;//就诊地点

    QLabel *status;//订单状态
};


//就诊人表项类

//身份证类型：居民身份证、护照、港澳居民来往内地通行证、台湾居民来往大陆通行证
enum ID_TYPE{DALU, HUZHAO, GANGAO, TAIWAN};

class patientItem : public QWidget {
    Q_OBJECT
public:
    explicit patientItem(const int &pn, const QString &n,const ID_TYPE &type,const QString &num,const QString &p,const QString &c ,QWidget *parent = nullptr);

    void switchToSelectable();//更改为可选模式

signals:

    void modBTNclicked(const int &patNum, const QString &name, const int &type,const QString &num, const QString &phone, const QString &cons);

    void delBTNclicked(const int &patNum);

    void itemClicked(const int &patNum);

protected:
    void mousePressEvent(QMouseEvent *event) override; // 鼠标点击事件

private:
    int patientNum;//就诊人编号

    QLabel *name; //就诊人姓名

    QLabel *idType;//身份证类型

    QLabel *idNum;//身份证

    QLabel *phone;//电话号码

    QPushButton *delBTN;//删除按钮

    QPushButton *modBTN;//修改按钮

    QLabel *cons;//体质

};


//医院表项类
class hospitalItem : public QWidget {
    Q_OBJECT
public:
    explicit hospitalItem(const int &i,const QString &n,const QString &r,const QString &a,const QString &l,const QString &path,QWidget *parent = nullptr);

signals:
    void clicked(const int id); // 点击信号

protected:
    void mousePressEvent(QMouseEvent *event) override; // 鼠标点击事件

private:
    int id;//医院id

    QLabel *name; //医院名

    QLabel *region;//医院所在地区

    QLabel *address;//地址

    QLabel *level;//医院等级

    QLabel *pic;//医院图片
};

struct doctor
{
    int id;

    QString name;

    QString hospital;

    QString title;

    QString department;

    QString path;

    QString intro;

    QString exp;

    int price;
};

//医生表项类
class doctorItem : public QWidget {
    Q_OBJECT
public:
    explicit doctorItem(const doctor doc, QWidget *parent = nullptr);

signals:
    void clicked(const doctor doc); // 点击信号

protected:
    void mousePressEvent(QMouseEvent *event) override; // 鼠标点击事件

private:
    int id;//医生id

    QLabel *name; //医生名字

    QLabel *hospital;//医院所在地区

    QLabel *title;//职称

    QLabel *department;//部门

    QLabel *pic;//医生图片

    QLabel *price;//挂号费

    doctor doc;
};
#endif // ANGITEM_H
