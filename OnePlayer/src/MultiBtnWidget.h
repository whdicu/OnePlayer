#pragma once

#include <QWidget>
#include "ui_MultiBtnWidget.h"

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
	int oldX_;
	int oldY_;
	int oldWidth_;
	int oldHeight_;
	bool isAnimateHide_;
};
