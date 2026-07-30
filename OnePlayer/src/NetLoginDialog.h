#pragma once

#include <QDialog>
#include "ui_NetLoginDialog.h"


class QTimer;


class NetLoginDialog : public QDialog
{
	Q_OBJECT

public:
	static NetLoginDialog* getInstance();

private slots:
	void on_btn_login_password_clicked();
	void on_btn_send_captcha_clicked();
	void on_btn_login_captcha_clicked();
	void on_btn_password_clicked();
	void on_btn_captcha_clicked();
	void on_btn_qr_clicked();
	void on_btn_login_clicked();
	void onCountdownTick();
	void onQrCheckTick();

private:
	NetLoginDialog(QWidget *parent = nullptr);
	~NetLoginDialog();

	void setActiveTypeButton(QPushButton* activeBtn);

	Ui::NetLoginDialogClass ui;
	QTimer* countdownTimer_ = nullptr;
	int countdown_ = 0;

	QTimer* qrTimer_ = nullptr;
	QString qrKey_;
};

#ifndef NET_LOGIN_DIALOG
#define NET_LOGIN_DIALOG NetLoginDialog::getInstance()
#endif
