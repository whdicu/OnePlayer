#include "NetLoginDialog.h"
#include "neteasehandler.h"
#include <QMessageBox>
#include <QRegularExpression>
#include "HDQt/HDWidget/DToast.hpp"


static NetLoginDialog* net_login_dialog = nullptr;
NetLoginDialog* NetLoginDialog::getInstance()
{
	if (nullptr == net_login_dialog)
		net_login_dialog = new NetLoginDialog;
	return net_login_dialog;
}

void NetLoginDialog::on_btn_send_captcha_clicked()
{
	QString phone = ui.edit_phone_email->text();
	quint64 i = phone.toULongLong();
	if (i < 10000000000ul || i > 19999999999ul)
	{
		QMessageBox::warning(this, tr("手机号错误"), tr("请输入正确的手机号"));
		return;
	}

	if (NETEASE_HANDLER->sendCaptcha(phone))
	{
		DToast::toast(tr("验证码发送成功"), DToast::FromBottom);
	}
	else
	{
		DToast::toast(tr("验证码发送失败"), DToast::FromBottom);
	}
}

void NetLoginDialog::on_btn_login_clicked()
{
	QString phoneOrEmail = ui.edit_phone_email->text();
	QString password = ui.edit_password->text();

	if (phoneOrEmail.contains('@'))
	{
		QRegularExpression regex("^\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}\\b$");
		QRegularExpressionMatch match = regex.match(phoneOrEmail);

		if (!match.hasMatch())
		{
			QMessageBox::warning(this, tr("邮箱错误"), tr("请输入正确的邮箱"));
			return;
		}

		if (NETEASE_HANDLER->loginEmail(phoneOrEmail, password))
		{
			// 登陆成功
		}
	}
	else
	{
		unsigned long long i = phoneOrEmail.toULongLong();
		if (i < 10000000000ul || i > 19999999999ul)
		{
			QMessageBox::warning(this, tr("手机号错误"), tr("请输入正确的手机号"));
			return;
		}

		//if (NETEASE_HANDLER->loginPhone(phoneOrEmail, password))
		if (NETEASE_HANDLER->loginCaptcha(phoneOrEmail, password))
		{
			// 登陆成功
		}
	}
}

NetLoginDialog::NetLoginDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

NetLoginDialog::~NetLoginDialog()
{

}
