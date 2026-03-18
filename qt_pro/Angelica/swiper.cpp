#include "swiper.h"

Swiper::Swiper(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    // 初始化动画
    animation.setTargetObject(this);
    animation.setPropertyName("curIndex");
    animation.setEasingCurve(QEasingCurve::OutQuart);
    animation.setDuration(1000);

//    // 初始化测试图片（保持原有逻辑）
//    int alpha = 255;
//    QList<QColor> colors = {
//        QColor(255,0,0,alpha),
//        QColor(0,255,0,alpha),
//        QColor(0,0,255,alpha),
//        QColor(255,255,0,alpha),
//        QColor(0,255,255,alpha)
//    };

//    for(int i=0; i<colors.size(); i++) {
//        ImageNode node;
//        node.img = QImage("C:/Users/86178/Desktop/640.png");
//        imageList.append(node);
//    }
}

void Swiper::addImage(const QString &path)
{
    ImageNode node;
    node.img = QImage(path);
    imageList.append(node);
}

void Swiper::beginPaint()
{
    calcImagePos();
    calcBtnPath();
}

void Swiper::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    painter.save();
    painter.translate(width()/2, height()/2-40);

    const double radiusX = width() * 0.4; // 根据控件大小动态调整
    const double radiusY = height() * 0.1;

    for(int i=0; i<drawList.size(); i++) {
        const ImageNode &node = imageList[drawList[i]];
        QPointF center(node.xf * radiusX, node.zf * radiusY);

        QRectF imgRect(0, 0, 260, 180);
        imgRect.moveCenter(center);

        painter.setOpacity(node.alpha / 255.0);
        painter.drawImage(imgRect, node.img);
    }
    painter.restore();

    // 绘制指示器
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);
    for(int i=0; i<btnList.size(); i++) {
        painter.setBrush(i == setIndex ? Qt::black : Qt::gray);
        painter.drawEllipse(btnList[i]);
    }
}

void Swiper::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    // 动态调整按钮位置
    const int btnSize = qMin(width()/20, 14);
    const int btnSpace = btnSize * 1.5;
    const int totalWidth = btnSize * imageList.size() + btnSpace * (imageList.size()-1);

    btnList.resize(imageList.size());
    for(int i=0; i<imageList.size(); i++) {
        btnList[i] = QRectF(
            width()/2 - totalWidth/2 + i*(btnSize + btnSpace),
            height() - btnSize * 2,
            btnSize, btnSize
        );
    }
}

void Swiper::toPrev()
{
    animation.stop();
    setIndex--;
    //到头了，就切换到尾巴上，为了动画连续，startvalue也设置到尾巴上
    if(setIndex<0){
        setIndex=imageList.size()-1;
        animation.setStartValue(getCurIndex()+imageList.size());
    }else{
        animation.setStartValue(getCurIndex());
    }

    animation.setEndValue((double)setIndex);
    animation.start();
}

void Swiper::toNext()
{
    animation.stop();
    setIndex++;
    if(setIndex>imageList.size()-1){
        setIndex=0;
        animation.setStartValue(getCurIndex()-imageList.size());
    }else{
        animation.setStartValue(getCurIndex());
    }
    animation.setEndValue((double)setIndex);
    animation.start();
}

double Swiper::getCurIndex() const
{
    return curIndex;
}

void Swiper::setCurIndex(double index)
{
    curIndex=index;
    emit curIndexChanged();
    //属性动画设置值会调用该接口，此处计算位置并刷新
    calcImagePos();
    update();
}

void Swiper::calcImagePos()
{
    if(imageList.isEmpty())
        return;
    drawList.resize(imageList.size());
    //每个图之间的角度间隔
    const double step=360.0/imageList.size();
    //绘制时会平移中心点，所以这里以0.0为中心点计算
    for(int i=0;i<imageList.size();i++)
    {
        ImageNode &node=imageList[i];
        //0度为0，+90度是让当前图z值为1
        const double degree=90+step*i-curIndex*step;
        const double radians=qDegreesToRadians(degree);
        //取反则为顺时针变化
        node.xf=-cos(radians);
        node.zf=sin(radians);
        //存下标用于计算堆叠顺序
        drawList[i]=i;
    }

    // 新透明度计算逻辑（结合空间位置和视觉焦点）
    const double fadeStartZ = 0.3;  // z轴开始淡出的阈值
    const double fadeRangeZ = 0.4; // z轴淡出范围
    const double activeRange = 1.2; // 可见区域范围（当前图±1.2索引）

    // 创建缓动曲线对象
    QEasingCurve zCurve(QEasingCurve::OutQuad);
    QEasingCurve indexCurve(QEasingCurve::InOutQuad);

    for(int i=0; i<imageList.size(); i++) {
        ImageNode &node = imageList[i];

        // 计算循环索引差（考虑动画中间值）
        double realIndex = curIndex + (i - static_cast<int>(curIndex)) * 0.2;
        double delta = qAbs(i - realIndex);
        delta = qMin(delta, imageList.size() - delta); // 处理循环差值

        // 使用曲线对象计算进度值
        double zAlpha = zCurve.valueForProgress(
            qBound(0.0, (node.zf - fadeStartZ)/fadeRangeZ, 1.0)
        );

        double indexAlpha = indexCurve.valueForProgress(
            qBound(0.0, 1.0 - (delta/activeRange), 1.0)
        );

        // 复合透明度计算（权重可调）
        double finalAlpha = zAlpha * indexAlpha;

        // 设置透明度范围（当前图255，两侧64，其他0）
        if(delta < 0.3) { // 当前图区域
            node.alpha = 255;
        } else if(delta < 1.5) { // 相邻图区域
            node.alpha = static_cast<int>(64 + 191 * finalAlpha);
        } else { // 不可见图区域
            node.alpha = 0;
        }

        // 强制后方不可见（即使索引差符合条件）
        if(node.zf < 0.1) node.alpha = 0;
    }

    //根据z排堆叠顺序
    std::sort(drawList.begin(),drawList.end(),
              [this](int a, int b){
        return imageList.at(a).zf<imageList.at(b).zf;
    });

}

void Swiper::calcBtnPath()
{
    if(imageList.isEmpty())
        return;
    //按钮也可以根据curIndex值来做过渡动画，略
    btnList.resize(imageList.size());
    //绘制位置的规则自己随便定
    int w=rect().width();
    int h=rect().height();
    //底部画几个圆圈
    int btn_space=20;
    int btn_width=14;
    int bar_width=btn_width*imageList.size()+btn_space*(imageList.size()-1);
    for(int i=0;i<imageList.size();i++)
    {
        btnList[i]=QRectF(w/2-bar_width/2+(btn_space+btn_width)*i,
                          h/2+100,
                          btn_width,btn_width);
    }
}

Swiper::~Swiper()
{
    //避免结束程序stop时异常
    swipTimer.disconnect(this);
    animation.disconnect(this);
    swipTimer.stop();
    animation.stop();
}
