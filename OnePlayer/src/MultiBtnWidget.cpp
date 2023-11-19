#include "MultiBtnWidget.h"
#include "OnePlayerStruct.h"
#include <QPropertyAnimation>


MultiBtnWidget::MultiBtnWidget(QWidget *parent)
	: QWidget(parent)
    , isAnimateHide_(false)
{
	ui.setupUi(this);

    animation_ = new QPropertyAnimation(this, "geometry");
    animation_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);
    animation_->setEasingCurve(QEasingCurve::InOutQuad);
    connect(animation_, &QPropertyAnimation::finished, this, [this]()
    {
        isAnimateHide_ = !isAnimateHide_;
        //animation->deleteLater();
    });
}

MultiBtnWidget::~MultiBtnWidget()
{}

void MultiBtnWidget::animationHide()
{
    animation_->setStartValue(QRect(x(), y(), width(), height()));
    animation_->setEndValue(QRect(MORE_BTN_WIDGET_X + MORE_BTN_WIDGET_WIDTH
        , MORE_BTN_WIDGET_Y, 0, MORE_BTN_WIDGET_HEIGHT));
    animation_->start();
}

void MultiBtnWidget::animationShow()
{
    animation_->setStartValue(QRect(x(), y(), width(), height()));
    animation_->setEndValue(QRect(MORE_BTN_WIDGET_X, MORE_BTN_WIDGET_Y
        , MORE_BTN_WIDGET_WIDTH, MORE_BTN_WIDGET_HEIGHT));
    animation_->start();
}
