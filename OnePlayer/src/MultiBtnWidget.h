#pragma once

#include <QWidget>
#include "ui_MultiBtnWidget.h"

class QPropertyAnimation;

class MultiBtnWidget : public QWidget
{
	Q_OBJECT

public:
	MultiBtnWidget(QWidget *parent = nullptr);
	~MultiBtnWidget();
	void animationHide();
	void animationShow();
	// 直接move到Hide位置
	void moveToHide();

	bool isAnimateHide() { return isAnimateHide_; }

private:
	Ui::MultiBtnWidgetClass ui;
	QPropertyAnimation* animation_;
	// 内部按钮动画
	QPropertyAnimation* animationAdd_;
	QPropertyAnimation* animationSetting_;
	QPropertyAnimation* animationSetting2_;
	QPropertyAnimation* animationSetting3_;
	QPropertyAnimation* animationSetting4_;
	QPropertyAnimation* animationSetting5_;
	QPropertyAnimation* animationSetting6_;
	bool isAnimateHide_;
};
