/*
 * QNChatMessage类，用于实现AI问答对话框
*/

#ifndef QNCHATMESSAGE_H
#define QNCHATMESSAGE_H

#include <QWidget>
#include <QFontMetrics>
#include <QPaintEvent>
#include <QDateTime>
#include <QPainter>
#include <QMovie>
#include <QLabel>
#include <QDebug>

class QPaintEvent;
class QPainter;
class QLabel;
class QMovie;

class QNChatMessage : public QWidget
{
    Q_OBJECT
public:
    explicit QNChatMessage(QWidget *parent = nullptr);

    enum User_Type{
        User_System,//系统
        User_Me,    //自己
        User_AI,   //AI
    };

    void setTextSuccess();
    void setText(QString text, QSize allSize, User_Type userType);

    //使用类前需要先设置头像路径
    static void setMyHead(const QString &path);
    static void setAIHead(const QString &path);

    QSize getRealString(QString src);
    QSize fontRect(QString str);

    inline QString text() {return m_msg;}
    inline User_Type userType() {return m_userType;}
protected:
    void paintEvent(QPaintEvent *event);
private:
    QString m_msg;
    QSize m_allSize;
    User_Type m_userType = User_System;

    int m_kuangWidth;
    int m_textWidth;
    int m_spaceWid;
    int m_lineHeight;

    static QString userPM_path;
    static QString AIPM_path;

    QPixmap m_leftPixmap;
    QPixmap m_rightPixmap;

    QRect m_iconLeftRect;
    QRect m_iconRightRect;
    QRect m_sanjiaoLeftRect;
    QRect m_sanjiaoRightRect;
    QRect m_kuangLeftRect;
    QRect m_kuangRightRect;
    QRect m_textLeftRect;
    QRect m_textRightRect;
    QLabel* m_loading = Q_NULLPTR;
    bool m_isSending = false;
};

#endif // QNCHATMESSAGE_H
