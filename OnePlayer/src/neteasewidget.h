#pragma once
#include <QWidget>
#include "ui_neteasewidget.h"

class NeteaseWidget : public QWidget
{
	Q_OBJECT

public:
	NeteaseWidget(QWidget* parent = nullptr);
	~NeteaseWidget();

private:

	Ui::NeteaseWidget ui;
};

