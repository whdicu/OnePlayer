#include "MultiBtnWidget.h"
#include "OnePlayerStruct.h"
#include <QPropertyAnimation>
#include <QTimer>


MultiBtnWidget::MultiBtnWidget(QWidget *parent)
	: QWidget(parent)
    , isAnimateHide_(false)
{
	ui.setupUi(this);

    animation_ = new QPropertyAnimation(this, "geometry");
    animation_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);
    animation_->setEasingCurve(QEasingCurve::OutCubic);
}

MultiBtnWidget::~MultiBtnWidget()
{}

void MultiBtnWidget::animationHide()
{
	isAnimateHide_ = true;
	animation_->stop();
    animation_->setStartValue(QRect(x(), y(), width(), height()));
    animation_->setEndValue(QRect(MORE_BTN_WIDGET_X, MORE_BTN_WIDGET_Y + MORE_BTN_WIDGET_HEIGHT
		, MORE_BTN_WIDGET_WIDTH, MORE_BTN_WIDGET_HEIGHT));

	animation_->start();
}

void MultiBtnWidget::animationShow()
{
	isAnimateHide_ = false;
	animation_->stop();
    animation_->setStartValue(QRect(x(), y(), width(), height()));
    animation_->setEndValue(QRect(MORE_BTN_WIDGET_X, MORE_BTN_WIDGET_Y
        , MORE_BTN_WIDGET_WIDTH, MORE_BTN_WIDGET_HEIGHT));

	animation_->start();
}
