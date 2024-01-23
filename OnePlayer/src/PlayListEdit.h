#pragma once

#include <QWidget>
#include "ui_PlayListEdit.h"

class PlayListEdit : public QWidget
{
	Q_OBJECT

public:
	PlayListEdit(QWidget *parent = Q_NULLPTR);
	~PlayListEdit();

signals:
	void sigAdd(const QString& playListName);

private slots:
	void on_btn_add_clicked();

private:
	Ui::PlayListEdit ui;
};
