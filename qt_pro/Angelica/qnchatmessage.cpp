/*
 * QNChatMessage类，参考自Github:https://github.com/ShaShiDiZhuanLan/Demo_MessageChat_Qt
*/

#include "qnchatmessage.h"

QString QNChatMessage::userPM_path="";
QString QNChatMessage::AIPM_path="";

QNChatMessage::QNChatMessage(QWidget *parent) : QWidget(parent)
{
    QFont te_font = this->font();
    te_font.setFamily("微软雅黑");
    te_font.setPointSize(10);
    this->setFont(te_font);
    m_leftPixmap = QPixmap(AIPM_path);
    m_rightPixmap = QPixmap(userPM_path);
    m_loading = new QLabel(this);
    m_loading->resize(35,35);
    m_loading->setScaledContents(true);
    m_loading->setAttribute(Qt::WA_TranslucentBackground , true);
    m_loading->setAutoFillBackground(false);
}

void QNChatMessage::setMyHead(const QString &path)
{
    userPM_path = path;
}

void QNChatMessage::setAIHead(const QString &path)
{
    AIPM_path = path;
}

void QNChatMessage::setTextSuccess()
{
    m_loading->hide();
    m_isSending = true;
}

void QNChatMessage::setText(QString text, QSize allSize, QNChatMessage::User_Type userType)
{
    m_msg = text;
    m_userType = userType;
    m_allSize = allSize;
    if(userType == User_Me) {
        if(!m_isSending) {
            m_loading->move(m_kuangRightRect.x() - m_loading->width() - 10, m_kuangRightRect.y()+m_kuangRightRect.height()/2- m_loading->height()/2);
            m_loading->show();
        }
    } else {
        m_loading->hide();
    }

    this->update();
}

QSize QNChatMessage::fontRect(QString str)
{
    m_msg = str;
    int minHei = 30;
    int iconWH = 40;
    int iconSpaceW = 20;
    int iconRectW = 5;
    int iconTMPH = 10;
    int sanJiaoW = 6;
    int kuangTMP = 20;
    int textSpaceRect = 12;
    m_kuangWidth = this->width() - kuangTMP - 2*(iconWH+iconSpaceW+iconRectW);
    m_textWidth = m_kuangWidth - 2*textSpaceRect;
    m_spaceWid = this->width() - m_textWidth;
    m_iconLeftRect = QRect(iconSpaceW, iconTMPH, iconWH, iconWH);
    m_iconRightRect = QRect(this->width() - iconSpaceW - iconWH-3, iconTMPH, iconWH, iconWH);

    QSize size = getRealString(m_msg); // 整个的size

    //qDebug() << "fontRect Size:" << size;
    int hei = size.height() < minHei ? minHei : size.height();

    m_sanjiaoLeftRect = QRect(iconWH+iconSpaceW+iconRectW, m_lineHeight/2, sanJiaoW, hei - m_lineHeight);
    m_sanjiaoRightRect = QRect(this->width() - iconRectW - iconWH - iconSpaceW - sanJiaoW, m_lineHeight/2, sanJiaoW, hei - m_lineHeight);

    if(size.width() < (m_textWidth+m_spaceWid)) {
        m_kuangLeftRect.setRect(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), m_lineHeight/4*3, size.width()-m_spaceWid+2*textSpaceRect, hei-m_lineHeight);
        m_kuangRightRect.setRect(this->width() - size.width() + m_spaceWid - 2*textSpaceRect - iconWH - iconSpaceW - iconRectW - sanJiaoW,
                                 m_lineHeight/4*3, size.width()-m_spaceWid+2*textSpaceRect, hei-m_lineHeight);
    } else {
        m_kuangLeftRect.setRect(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), m_lineHeight/4*3, m_kuangWidth, hei-m_lineHeight);
        m_kuangRightRect.setRect(iconWH + kuangTMP + iconSpaceW + iconRectW - sanJiaoW, m_lineHeight/4*3, m_kuangWidth, hei-m_lineHeight);
    }
    m_textLeftRect.setRect(m_kuangLeftRect.x()+textSpaceRect,m_kuangLeftRect.y()+iconTMPH,
                           m_kuangLeftRect.width()-2*textSpaceRect,m_kuangLeftRect.height()-2*iconTMPH);
    m_textRightRect.setRect(m_kuangRightRect.x()+textSpaceRect,m_kuangRightRect.y()+iconTMPH,
                            m_kuangRightRect.width()-2*textSpaceRect,m_kuangRightRect.height()-2*iconTMPH);

    return QSize(size.width(), hei);
}

QSize QNChatMessage::getRealString(QString src)
{
    QFontMetricsF fm(this->font());
    m_lineHeight = fm.lineSpacing();
    int nCount = src.count("\n");
    int nMaxWidth = 0;
    if(nCount == 0) {
        nMaxWidth = fm.width(src);
        QString value = src;
        if(nMaxWidth > m_textWidth) {
            nMaxWidth = m_textWidth;
            int size = m_textWidth / fm.width(" ");
            int num = fm.width(value) / m_textWidth;
            int ttmp = num*fm.width(" ");
            num = ( fm.width(value) ) / m_textWidth;
            nCount += num;
            QString temp = "";
            for(int i = 0; i < num; i++) {
                temp += value.mid(i*size, (i+1)*size) + "\n";
            }
            src.replace(value, temp);
        }
    } else {
        for(int i = 0; i < (nCount + 1); i++) {
            QString value = src.split("\n").at(i);
            nMaxWidth = fm.width(value) > nMaxWidth ? fm.width(value) : nMaxWidth;
            if(fm.width(value) > m_textWidth) {
                nMaxWidth = m_textWidth;
                int size = m_textWidth / fm.width(" ");
                int num = fm.width(value) / m_textWidth;
                num = ((i+num)*fm.width(" ") + fm.width(value)) / m_textWidth;
                nCount += num;
                QString temp = "";
                for(int i = 0; i < num; i++) {
                    temp += value.mid(i*size, (i+1)*size) + "\n";
                }
                src.replace(value, temp);
            }
        }
    }
    return QSize(nMaxWidth+m_spaceWid, (nCount + 1) * m_lineHeight+2*m_lineHeight);
}

void QNChatMessage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);//消锯齿
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor("#f4f4f4")));

    if(m_userType == User_Type::User_AI) { // AI
        //头像
        painter.drawRoundedRect(m_iconLeftRect,m_iconLeftRect.width(),m_iconLeftRect.height());
        painter.drawPixmap(m_iconLeftRect, m_leftPixmap);

        //框加边
        QColor col_KuangB(234, 234, 234);
        painter.setBrush(QBrush(col_KuangB));
        painter.drawRoundedRect(m_kuangLeftRect.x()-1,m_kuangLeftRect.y()-1,m_kuangLeftRect.width()+2,m_kuangLeftRect.height()+2,4,4);
        //框
        QColor col_Kuang(255,255,255);
        painter.setBrush(QBrush(col_Kuang));
        painter.drawRoundedRect(m_kuangLeftRect,4,4);

        //三角
        QPointF points[3] = {
            QPointF(m_sanjiaoLeftRect.x(), 30),
            QPointF(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), 25),
            QPointF(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), 35),
        };
        QPen pen;
        pen.setColor(col_Kuang);
        painter.setPen(pen);
        painter.drawPolygon(points, 3);

        //三角加边
        QPen penSanJiaoBian;
        penSanJiaoBian.setColor(col_KuangB);
        painter.setPen(penSanJiaoBian);
        painter.drawLine(QPointF(m_sanjiaoLeftRect.x() - 1, 30), QPointF(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), 24));
        painter.drawLine(QPointF(m_sanjiaoLeftRect.x() - 1, 30), QPointF(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), 36));

        //内容
        QPen penText;
        penText.setColor(QColor(51,51,51));
        painter.setPen(penText);
        QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
        option.setWrapMode(QTextOption::WrapAnywhere);//WrapAtWordBoundaryOrAnywhere);
        painter.setFont(this->font());
        painter.drawText(m_textLeftRect, m_msg,option);
    }
    else if(m_userType == User_Type::User_Me) { // 自己
        //头像
        painter.drawRoundedRect(m_iconRightRect,m_iconRightRect.width(),m_iconRightRect.height());
        painter.drawPixmap(m_iconRightRect, m_rightPixmap);

        //框
        QColor col_Kuang(0,171,149);
        painter.setBrush(QBrush(col_Kuang));
        painter.drawRoundedRect(m_kuangRightRect,4,4);

        //三角
        QPointF points[3] = {
            QPointF(m_sanjiaoRightRect.x()+m_sanjiaoRightRect.width(), 30),
            QPointF(m_sanjiaoRightRect.x(), 25),
            QPointF(m_sanjiaoRightRect.x(), 35),
        };
        QPen pen;
        pen.setColor(col_Kuang);
        painter.setPen(pen);
        painter.drawPolygon(points, 3);

        //内容
        QPen penText;
        penText.setColor(Qt::white);
        painter.setPen(penText);
        QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        painter.setFont(this->font());
        painter.drawText(m_textRightRect,m_msg,option);
    }
}
