/*
 * ItchScaleWidget类，用于实现NRS色块
 */

#ifndef ITCHSCALEWIDGET_H
#define ITCHSCALEWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class scaleWidget : public QWidget {
    Q_OBJECT
public:
    explicit scaleWidget(QWidget* parent = nullptr);

    //设置两侧的文本
    void setText(const QString &left, const QString &right);

    //设置是否可用
    void setEnable(const bool &f);

signals:
    //分数选择信号
    void scoreSelected(int score);

private:
    QHBoxLayout* m_layout;

    //计算颜色过渡
    QColor calculateColor(int index) const;

    //左右侧标签
    QLabel *leftLabel;
    QLabel *rightLabel;

    //可用
    bool isEnable=true;
};

#endif // ITCHSCALEWIDGET_H
