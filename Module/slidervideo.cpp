#include "slidervideo.h"

#include <QMouseEvent>
#include <QStyle>

SliderVideo::SliderVideo(QWidget *parent) : QSlider(parent)
{
}

//设置当鼠标点击右键的时候，让侧边出现的位置，吟隐藏
void SliderVideo::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::MidButton)       // 右键或中键点击时跳转到鼠标点击位置
    {
        int value = QStyle::sliderValueFromPosition(this->minimum(), this->maximum(), event->pos().x(), this->width());
        this->setValue(value);
    }
    QSlider::mousePressEvent(event);
}
