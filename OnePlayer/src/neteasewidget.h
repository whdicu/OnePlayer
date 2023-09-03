#pragma once
#include <QWidget>
#include "ui_neteasewidget.h"

class NeteaseWidget : public QWidget
{
	Q_OBJECT

public:
	NeteaseWidget(QWidget* parent = nullptr);
	~NeteaseWidget();

private slots:
	void on_btn_sign_up_clicked();

	// µÇÂ¼
	void slotLogin();

private:

	Ui::NeteaseWidget ui;
};

