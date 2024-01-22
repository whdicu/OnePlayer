#pragma once

#include <QWidget>
#include "ui_PlayListEdit.h"

class PlayListEdit : public QWidget
{
	Q_OBJECT

public:
	PlayListEdit(QWidget *parent = Q_NULLPTR);
	~PlayListEdit();

private:
	Ui::PlayListEdit ui;
};
