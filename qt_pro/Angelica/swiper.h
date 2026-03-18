/*
 * 绘制轮播图
 */

#ifndef SWIPER_H
#define SWIPER_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QPainter>
#include <cmath>
#include <QtMath>
#include <QDebug>
#include <QPainterPath>
#include <QMouseEvent>
#include <QImage>
#include <QTimer>

class Swiper : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double curIndex READ getCurIndex WRITE setCurIndex NOTIFY curIndexChanged)
public:
    explicit Swiper(QWidget *parent = nullptr);
    ~Swiper();

    double getCurIndex() const;

    void setCurIndex(double index);

    //开始绘制
    void beginPaint();

    void addImage(const QString &path);

public slots:
    void toPrev();
    void toNext();

signals:
    void curIndexChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    struct ImageNode { // 图片节点结构体
        QImage img;
        double xf = 0;
        double zf = 0;
        int alpha = 255;
    };

    //当前图片列表
    QVector<ImageNode> imageList;
    //按钮位置
    QVector<QRectF> btnList;
    //图片z堆叠顺序
    QVector<int> drawList;
    //当前设置的index
    int setIndex{0};
    //属性动画绘制用到的index
    double curIndex{0.0};
    //旋转动画
    QPropertyAnimation animation;
    //自动切换定时器
    QTimer swipTimer;

    void calcImagePos();          // 计算图片位置
    void calcBtnPath();           // 计算按钮位置
    double getCyclicDelta(int index) const; // 循环差值计算
};

#endif // SWIPER_H
