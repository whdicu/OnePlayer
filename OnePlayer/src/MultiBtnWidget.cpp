#include "MultiBtnWidget.h"
#include <QPropertyAnimation>
#include "OnePlayerStruct.h"

MultiBtnWidget::MultiBtnWidget(QWidget *parent)
	: QWidget(parent)
    , oldX_(-1)
    , oldY_(-1)
    , oldWidth_(-1)
    , oldHeight_(-1)
    , isAnimateHide_(false)
{
	ui.setupUi(this);
}

MultiBtnWidget::~MultiBtnWidget()
{}

void MultiBtnWidget::animationHide()
{
    oldX_ = x();
    oldY_ = y();
    oldWidth_ = width();
    oldHeight_ = height();

    QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(animation, &QPropertyAnimation::finished, this, [this, animation]()
    {
        isAnimateHide_ = true;
        animation->deleteLater();
    });

    animation->setStartValue(QRect(oldX_, oldY_, oldWidth_, oldHeight_));
    animation->setEndValue(QRect(oldX_ + oldWidth_, oldY_, 0, oldHeight_));
    animation->start();
}

void MultiBtnWidget::animationShow()
{
    if (oldWidth_ == -1)
        return;

    QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(animation, &QPropertyAnimation::finished, this, [this, animation]()
    {
        isAnimateHide_ = false;
        animation->deleteLater();
    });

    animation->setStartValue(QRect(x(), y(), 0, height()));
    animation->setEndValue(QRect(oldX_, oldY_, oldWidth_, oldHeight_));
    animation->start();
}
