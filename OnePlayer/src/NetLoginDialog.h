#pragma once

#include <QDialog>
#include "ui_NetLoginDialog.h"

class NetLoginDialog : public QDialog
{
	Q_OBJECT

public:
	static NetLoginDialog* getInstance();

private slots:
	void on_btn_sign_up_clicked();
	void on_btn_login_clicked();

private:
	NetLoginDialog(QWidget *parent = nullptr);
	~NetLoginDialog();

	Ui::NetLoginDialogClass ui;
};

#ifndef NET_LOGIN_DIALOG
#define NET_LOGIN_DIALOG NetLoginDialog::getInstance()
#endif
