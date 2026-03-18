#include "scalewidget.h"
#include <QLabel>
#include <QSpacerItem>

scaleWidget::scaleWidget(QWidget* parent) : QWidget(parent)
{

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(5);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    const int btnWidth = 30;
    const int btnHeight = 50;

    for (int i=0; i<11; i++)
    {
        QPushButton* btn = new QPushButton(this);
        btn->setFixedSize(btnWidth, btnHeight);

        // 设置颜色
        QColor baseColor = calculateColor(i);
        btn->setStyleSheet(QString(
            "QPushButton {"
            "  background-color: %1;"
            "  border: none;"
            "  border-radius: 3px;"
            "}"
            "QPushButton:hover {"
            "  background-color: %2;"
            "}"
            "QPushButton:pressed {"
            "  background-color: %3;"
            "}"
        ).arg(baseColor.name(),
              baseColor.darker(110).name(),
              baseColor.darker(130).name()));

        btn->setCursor(Qt::PointingHandCursor);

        connect(btn,&QPushButton::clicked,[=]{
            if(isEnable)
                emit scoreSelected(i);
        });
        buttonLayout->addWidget(btn);
    }
    mainLayout->addLayout(buttonLayout);

    // === 标签布局 ===
    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->setContentsMargins(0, 0, 0, 0);

    // 左侧"不痒"标签
    leftLabel = new QLabel(this);
    leftLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    leftLabel->setStyleSheet("color: #666666; font: 14px 微软雅黑; font-weight:bold; ");

    // 中间占位弹簧
    QSpacerItem* spacer = new QSpacerItem(
        0, 0,
        QSizePolicy::Expanding,
        QSizePolicy::Minimum
    );

    // 右侧"极痒"标签
    rightLabel = new QLabel(this);
    rightLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    rightLabel->setStyleSheet("color: #666666; font: 14px 微软雅黑; font-weight:bold;");

    // 计算标签位置偏移（根据按钮宽度和间距）
    const int labelOffset = btnWidth/2 - 14; // 微调对齐位置
    labelLayout->addWidget(leftLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    labelLayout->addItem(spacer);
    labelLayout->addWidget(rightLabel, 0, Qt::AlignRight | Qt::AlignTop);

    // 设置边距实现精确对齐
    labelLayout->setContentsMargins(
        labelOffset, 0,  // 左：第一个按钮中心偏移
        labelOffset, 0); // 右：最后一个按钮中心偏移

    mainLayout->addLayout(labelLayout);
}

QColor scaleWidget::calculateColor(int index) const
{
    // 定义起始颜色（浅粉色）和结束颜色（深红色）
    const QColor startColor(255, 230, 230);
    const QColor endColor(200, 0, 0);

    // 计算过渡比例
    float ratio = static_cast<float>(index) / 10.0f;

    // 线性插值计算颜色分量
    return QColor(
        startColor.red() + (endColor.red() - startColor.red()) * ratio,
        startColor.green() + (endColor.green() - startColor.green()) * ratio,
        startColor.blue() + (endColor.blue() - startColor.blue()) * ratio
    );
}

void scaleWidget::setText(const QString &left, const QString &right)
{
    leftLabel->setText(left);
    rightLabel->setText(right);
}

void scaleWidget::setEnable(const bool &f)
{
    isEnable=f;
}
