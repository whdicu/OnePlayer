#include "NetLoginDialog.h"
#include "ImageHandler.h"
#include "neteasehandler.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTimer>
#include "DToast.h"


static NetLoginDialog* net_login_dialog = nullptr;
NetLoginDialog* NetLoginDialog::getInstance()
{
	if (nullptr == net_login_dialog)
		net_login_dialog = new NetLoginDialog;
	return net_login_dialog;
}

void NetLoginDialog::on_btn_login_password_clicked()
{
	QString phoneOrEmail = ui.edit_phone_email->text();
	QString password = ui.edit_password->text();

	if (phoneOrEmail.contains('@'))
	{
		QRegularExpression regex("^\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}\\b$");
		QRegularExpressionMatch match = regex.match(phoneOrEmail);

		if (!match.hasMatch())
		{
			DToast::toast(tr("请输入正确的邮箱"), DToast::FromBottom);
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
			DToast::toast(tr("请输入正确的手机号"), DToast::FromBottom);
			return;
		}

		// 手机登录
		//bool ret = NETEASE_HANDLER->loginPhone(phoneOrEmail, password);

		QString errMsg;

		bool ret2 = NETEASE_HANDLER->loginPhonePassword(phoneOrEmail, password, errMsg);
		if (ret2)
		{
			return;
		}

		qWarning() << __FUNCTION__ << "login faild! err:" << errMsg;
		DToast::toast(errMsg, DToast::FromBottom);
	}
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

	ui.btn_send_captcha->setEnabled(false);
	countdown_ = 60;
	ui.btn_send_captcha->setText(tr("发送验证码(%1)").arg(countdown_));
	countdownTimer_->start(1000);
}

void NetLoginDialog::on_btn_login_captcha_clicked()
{
	QString phone = ui.edit_phone->text();
	QString password = ui.edit_password->text();

	unsigned long long i = phone.toULongLong();
	if (i < 10000000000ul || i > 19999999999ul)
	{
		DToast::toast(tr("请输入正确的手机号"), DToast::FromBottom);
		return;
	}

	// 先检验验证码是否正确
	QString errMsg;
	bool ret1 = NETEASE_HANDLER->checkCaptcha(phone, password, errMsg);

	if (ret1)
	{
		bool ret2 = NETEASE_HANDLER->loginPhoneCaptcha(phone, password, errMsg);
		if (ret2)
		{
			return;
		}
	}

	qWarning() << __FUNCTION__ << "login faild! err:" << errMsg;
	DToast::toast(errMsg, DToast::FromBottom);
}

void NetLoginDialog::onCountdownTick()
{
	countdown_--;
	if (countdown_ <= 0)
	{
		countdownTimer_->stop();
		ui.btn_send_captcha->setEnabled(true);
		ui.btn_send_captcha->setText(tr("发送验证码"));
	}
	else
	{
		ui.btn_send_captcha->setText(tr("发送验证码(%1)").arg(countdown_));
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
			DToast::toast(tr("请输入正确的邮箱"), DToast::FromBottom);
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
			DToast::toast(tr("请输入正确的手机号"), DToast::FromBottom);
			return;
		}

		// 手机登录
		//bool ret = NETEASE_HANDLER->loginPhone(phoneOrEmail, password);
		
		// 先检验验证码是否正确
		QString errMsg;
		bool ret1 = NETEASE_HANDLER->checkCaptcha(phoneOrEmail, password, errMsg);

		if (ret1)
		{
			bool ret2 = NETEASE_HANDLER->loginPhoneCaptcha(phoneOrEmail, password, errMsg);
			if (ret2)
			{
				return;
			}
		}
		
		qWarning() << __FUNCTION__ << "login faild! err:" << errMsg;
		DToast::toast(errMsg, DToast::FromBottom);
	}
}

void NetLoginDialog::on_btn_password_clicked()
{
	ui.stackedWidget->setCurrentIndex(0);
	setActiveTypeButton(ui.btn_password);
}

void NetLoginDialog::on_btn_captcha_clicked()
{
	ui.stackedWidget->setCurrentIndex(1);
	setActiveTypeButton(ui.btn_captcha);
}

void NetLoginDialog::on_btn_qr_clicked()
{
	ui.stackedWidget->setCurrentIndex(2);
	setActiveTypeButton(ui.btn_qr);

	qrKey_ = NETEASE_HANDLER->getQrKey();
	if (qrKey_.isEmpty())
	{
		DToast::toast(tr("获取二维码Key失败"), DToast::FromBottom);
		return;
	}

	QString qrUrl = NETEASE_HANDLER->getQrImageUrl(qrKey_);
	if (qrUrl.isEmpty())
	{
		DToast::toast(tr("获取二维码失败"), DToast::FromBottom);
		return;
	}

	//QDesktopServices::openUrl(QUrl(qrUrl));

	auto callBack = new ImageDownloadCallBack;
	connect(callBack, &ImageDownloadCallBack::sigImageSet, this, [this, callBack](SharedImage image)
	{
		ui.label_qr->setPixmap(QPixmap::fromImage(*image).scaled(
			ui.label_qr->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		callBack->deleteLater();
	});
	ImageHandler::downloadImage(qrUrl, callBack);

	qrTimer_->start(3000);
}

void NetLoginDialog::onQrCheckTick()
{
	int status = NETEASE_HANDLER->checkQrStatus(qrKey_);
	switch (status)
	{
	case 800:
		qrTimer_->stop();
		DToast::toast(tr("二维码已过期，请刷新"), DToast::FromBottom);
		break;
	case 801:
		// 等待扫码
		break;
	case 802:
		// 待确认
		break;
	case 803:
		qrTimer_->stop();
		DToast::toast(tr("登录成功"), DToast::FromBottom);
		accept();
		break;
	}
}

void NetLoginDialog::setActiveTypeButton(QPushButton* activeBtn)
{
	const QString activeColor = "#b6d1c8";
	const QString inactiveColor = "rgba(182, 209, 200, .6)";

	ui.btn_password->setStyleSheet(QString("background-color: %1;").arg(
		activeBtn == ui.btn_password ? activeColor : inactiveColor));
	ui.btn_captcha->setStyleSheet(QString("background-color: %1;").arg(
		activeBtn == ui.btn_captcha ? activeColor : inactiveColor));
	ui.btn_qr->setStyleSheet(QString("background-color: %1;").arg(
		activeBtn == ui.btn_qr ? activeColor : inactiveColor));
}

NetLoginDialog::NetLoginDialog(QWidget *parent)
	: QDialog(parent)
	, countdownTimer_(new QTimer(this))
	, qrTimer_(new QTimer(this))
{
	ui.setupUi(this);

	// 去除dialog右上角问号按钮
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui.btn_qr->hide();

	connect(countdownTimer_, &QTimer::timeout, this, &NetLoginDialog::onCountdownTick);
	connect(qrTimer_, &QTimer::timeout, this, &NetLoginDialog::onQrCheckTick);

	ui.btn_password->click();
}

NetLoginDialog::~NetLoginDialog()
{

}
