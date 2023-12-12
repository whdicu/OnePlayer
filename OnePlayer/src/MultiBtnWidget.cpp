#include "MultiBtnWidget.h"
#include "OnePlayerStruct.h"
#include <QPropertyAnimation>
#include <QTimer>


MultiBtnWidget::MultiBtnWidget(QWidget *parent)
	: QWidget(parent)
    , isAnimateHide_(false)
{
	ui.setupUi(this);

	// 内部按钮的动画
	animationAdd_ = new QPropertyAnimation(ui.btn_add, "pos");
	animationAdd_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * 44 / 44);
	animationAdd_->setEasingCurve(QEasingCurve::OutCubic);

	animationSetting_ = new QPropertyAnimation(ui.btn_setting, "pos");
	animationSetting_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * 38 / 44);
	animationSetting_->setEasingCurve(QEasingCurve::OutCubic);

	animationSetting2_ = new QPropertyAnimation(ui.btn_setting_2, "pos");
	animationSetting2_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * 32 / 44);
	animationSetting2_->setEasingCurve(QEasingCurve::OutCubic);

	animationSetting3_ = new QPropertyAnimation(ui.btn_setting_3, "pos");
	animationSetting3_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * 26 / 44);
	animationSetting3_->setEasingCurve(QEasingCurve::OutCubic);
	
	animationSetting4_ = new QPropertyAnimation(ui.btn_setting_4, "pos");
	animationSetting4_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * 20 / 44);
	animationSetting4_->setEasingCurve(QEasingCurve::OutCubic);

	animationSetting5_ = new QPropertyAnimation(ui.btn_setting_5, "pos");
	animationSetting5_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * 14 / 44);
	animationSetting5_->setEasingCurve(QEasingCurve::OutCubic);

	animationSetting6_ = new QPropertyAnimation(ui.btn_setting_6, "pos");
	animationSetting6_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * 8 / 44);
	animationSetting6_->setEasingCurve(QEasingCurve::OutCubic);
}

MultiBtnWidget::~MultiBtnWidget()
{}

void MultiBtnWidget::animationHide()
{
	isAnimateHide_ = true;
	animationAdd_->stop();
	animationSetting_->stop();
	animationSetting2_->stop();
	animationSetting3_->stop();
	animationSetting4_->stop();
	animationSetting5_->stop();
	animationSetting6_->stop();

	QPoint endPoint(0, MORE_BTN_WIDGET_HEIGHT + 40);
	animationAdd_->setStartValue(QPoint(ui.btn_add->x(), ui.btn_add->y()));
	animationAdd_->setEndValue(endPoint);

	animationSetting_->setStartValue(QPoint(ui.btn_setting->x(), ui.btn_setting->y()));
	animationSetting_->setEndValue(endPoint);

	animationSetting2_->setStartValue(QPoint(ui.btn_setting_2->x(), ui.btn_setting_2->y()));
	animationSetting2_->setEndValue(endPoint);

	animationSetting3_->setStartValue(QPoint(ui.btn_setting_3->x(), ui.btn_setting_3->y()));
	animationSetting3_->setEndValue(endPoint);

	animationSetting4_->setStartValue(QPoint(ui.btn_setting_4->x(), ui.btn_setting_4->y()));
	animationSetting4_->setEndValue(endPoint);

	animationSetting5_->setStartValue(QPoint(ui.btn_setting_5->x(), ui.btn_setting_5->y()));
	animationSetting5_->setEndValue(endPoint);

	animationSetting6_->setStartValue(QPoint(ui.btn_setting_6->x(), ui.btn_setting_6->y()));
	animationSetting6_->setEndValue(endPoint);

	animationAdd_->start();
	animationSetting_->start();
	animationSetting2_->start();
	animationSetting3_->start();
	animationSetting4_->start();
	animationSetting5_->start();
	animationSetting6_->start();

	QTimer::singleShot(MORE_BTN_WIDGET_ANIMATION_TIME, [this]()
	{
		if (isAnimateHide_)
			move(MORE_BTN_WIDGET_X, MORE_BTN_WIDGET_Y + MORE_BTN_WIDGET_HEIGHT);
	});
}

void MultiBtnWidget::animationShow()
{
	isAnimateHide_ = false;
	animationAdd_->stop();
	animationSetting_->stop();
	animationSetting2_->stop();
	animationSetting3_->stop();
	animationSetting4_->stop();
	animationSetting5_->stop();
	animationSetting6_->stop();

	move(MORE_BTN_WIDGET_X, MORE_BTN_WIDGET_Y);

	animationAdd_->setStartValue(QPoint(ui.btn_add->x(), ui.btn_add->y()));
	animationAdd_->setEndValue(QPoint(0, 0));

	animationSetting_->setStartValue(QPoint(ui.btn_setting->x(), ui.btn_setting->y()));
	animationSetting_->setEndValue(QPoint(0, 60));

	animationSetting2_->setStartValue(QPoint(ui.btn_setting_2->x(), ui.btn_setting_2->y()));
	animationSetting2_->setEndValue(QPoint(0, 120));

	animationSetting3_->setStartValue(QPoint(ui.btn_setting_3->x(), ui.btn_setting_3->y()));
	animationSetting3_->setEndValue(QPoint(0, 180));

	animationSetting4_->setStartValue(QPoint(ui.btn_setting_4->x(), ui.btn_setting_4->y()));
	animationSetting4_->setEndValue(QPoint(0, 240));

	animationSetting5_->setStartValue(QPoint(ui.btn_setting_5->x(), ui.btn_setting_5->y()));
	animationSetting5_->setEndValue(QPoint(0, 300));

	animationSetting6_->setStartValue(QPoint(ui.btn_setting_6->x(), ui.btn_setting_6->y()));
	animationSetting6_->setEndValue(QPoint(0, 360));

	animationAdd_->start();
	animationSetting_->start();
	animationSetting2_->start();
	animationSetting3_->start();
	animationSetting4_->start();
	animationSetting5_->start();
	animationSetting6_->start();
}

void MultiBtnWidget::moveToHide()
{
	isAnimateHide_ = true;
	move(MORE_BTN_WIDGET_X, MORE_BTN_WIDGET_Y + MORE_BTN_WIDGET_HEIGHT);

	QPoint endPoint(0, MORE_BTN_WIDGET_HEIGHT + 40);
	ui.btn_add->move(endPoint);
	ui.btn_setting->move(endPoint);
	ui.btn_setting_2->move(endPoint);
	ui.btn_setting_3->move(endPoint);
	ui.btn_setting_4->move(endPoint);
	ui.btn_setting_5->move(endPoint);
	ui.btn_setting_6->move(endPoint);
}
