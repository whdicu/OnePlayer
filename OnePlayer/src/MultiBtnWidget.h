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

signals:
	void sigBtnNeteaseClicked();
	void sigBtnSettingClicked();

private:
	Ui::MultiBtnWidgetClass ui;
	// 内部按钮动画
	QPropertyAnimation* animation1_;
	QPropertyAnimation* animation2_;
	QPropertyAnimation* animation3_;
	QPropertyAnimation* animation4_;
	QPropertyAnimation* animation5_;
	QPropertyAnimation* animation6_;
	QPropertyAnimation* animation7_;
	bool isAnimateHide_;
};
