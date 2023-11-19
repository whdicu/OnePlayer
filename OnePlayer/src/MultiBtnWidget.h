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
	bool isAnimateHide() { return isAnimateHide_; }

private:
	Ui::MultiBtnWidgetClass ui;

	QPropertyAnimation* animation_;
	bool isAnimateHide_;
};
