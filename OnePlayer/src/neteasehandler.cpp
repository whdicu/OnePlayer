#include "neteasehandler.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QNetworkReply>
#include "windows.h"


const static QString FIRST_URL = "http://127.0.0.1:3000";

static NeteaseHandler* netease_handler = nullptr;
NeteaseHandler* NeteaseHandler::getInstance()
{
	if (nullptr == netease_handler)
		netease_handler = new NeteaseHandler;
	return netease_handler;
}

void NeteaseHandler::deleteThis()
{
	stopApiExe();
}

bool NeteaseHandler::loginPhone(const QString& phone, const QString& password)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QString passwordMD5 = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();
	QString url = QString("/login/cellphone?timestamp=%1").arg(nowTime);  // 加上时间戳防止触发网易云API的缓存机制
	QString content = QString("phone=%1&md5_password=%2").arg(phone).arg(passwordMD5);
	DSharedPointer<QJsonObject> jo = execPost(url, content);

	/*if (jo->isEmpty())
	{
		QMessageBox::warning(nullptr, tr("登陆失败"), tr("网络太拥挤，请稍后再试"));
		return false;
	}*/

	int code = jo->value("code").toInt();
	QString message = jo->value("message").toString();
	switch (code)
	{
	case 502:
		QMessageBox::warning(nullptr, tr("登陆失败"), message);
		return false;
	}
	qDebug() << *jo;
	return true;
}

bool NeteaseHandler::loginEmail(const QString& email, const QString& password)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QString passwordMD5 = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();
	QString url = QString("/login?timestamp=%1").arg(nowTime);  // 加上时间戳防止触发网易云API的缓存机制
	QString content = QString("email=%1&md5_password=%2").arg(email).arg(passwordMD5);
	DSharedPointer<QJsonObject> jo = execPost(url, content);

	/*if (jo->isEmpty())
	{
		QMessageBox::warning(nullptr, tr("登陆失败"), tr("网络太拥挤，请稍后再试"));
		return false;
	}*/

	int code = jo->value("code").toInt();
	QString message = jo->value("message").toString();

	switch (code)
	{
	case 502:
		QMessageBox::warning(nullptr, tr("登陆失败"), message);
		return false;
	}
	qDebug() << *jo;
	return true;
}

DSharedPointer<QJsonObject> NeteaseHandler::execPost(const QString& url, const QString& content)
{
	QNetworkRequest request(FIRST_URL + url);
	QNetworkReply* reply = networkManager_->post(request, content.toUtf8());  // 发送 POST 请求
	DSharedPointer<QJsonObject> ret(new QJsonObject);

	QEventLoop loop;
	connect(reply, &QNetworkReply::finished, this, [reply, &ret, &loop]()
	{
		if (reply->error() == QNetworkReply::NoError)
		{
			QByteArray responseData = reply->readAll();
			reply->deleteLater();
			QJsonParseError parseError;
			QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &parseError);
			if (QJsonParseError::NoError != parseError.error)
			{
				qDebug() << "Json file analyze failed" << __FUNCTION__;
			}
			else
			{
				*ret = jsonDoc.object();
			}
		}
		else
		{
			// 处理错误情况
			reply->deleteLater();
		}
		loop.quit();
	});
	loop.exec();

	return ret;
}

NeteaseHandler::NeteaseHandler(QObject *parent)
	: QObject(parent)
	, apiProcess_(new QProcess(this))
	, networkManager_(new QNetworkAccessManager(this))
{
	//loginPhone("15557539750", "Whd2001129");
	startApiExe();
}

NeteaseHandler::~NeteaseHandler()
{
	deleteThis();
}

void NeteaseHandler::startApiExe()
{
	//apiThread_->start();
	apiProcess_->start("NeteaseCloudMusicApi-win.exe");
	//if (!ret)
	//{
	//	QMessageBox::warning(nullptr, tr("警告"), tr("启动网易云API程序失败"));
	//}
}

void NeteaseHandler::stopApiExe()
{
	apiProcess_->kill();
}

void NeteaseHandler::printJsonObject(const QJsonObject& obj, int space)
{
	/*if (obj.isEmpty())
		return;

	for (auto k : obj.keys())
	{
		QString sp(space, '\t');
		qDebug().noquote() << sp << k << ":" << ;
		printJsonObject(obj.value(k).toObject(), space+1);
	}*/
}
