#pragma once

#include <QDialog>
#include "ui_OneMessageBox.h"

enum OneMessageBoxType
{
	INFORMATION_BOX,
	WARNING_BOX,
	CRITICAL_BOX
};

enum OneMessageBoxButton
{
	NO_BTN = 0x00,
	ACCEPT_BTN = 0x01,
	CANCEL_BTN = 0x02,
	ALL_BTN = ACCEPT_BTN | CANCEL_BTN
};

class OneMessageBox : private QDialog
{
	Q_OBJECT

public:
	static int information(QWidget* parent, const QString& title, const QString& text, OneMessageBoxButton btn = ACCEPT_BTN);
	static int warning(QWidget* parent, const QString& title, const QString& text, OneMessageBoxButton btn = ACCEPT_BTN);
	static int critical(QWidget* parent, const QString& title, const QString& text, OneMessageBoxButton btn = ACCEPT_BTN);

	OneMessageBox(QWidget* parent, const QString& title, const QString& text, OneMessageBoxType type = INFORMATION_BOX, OneMessageBoxButton btn = ACCEPT_BTN);
	~OneMessageBox();

private:
	Ui::OneMessageBox ui;
};
