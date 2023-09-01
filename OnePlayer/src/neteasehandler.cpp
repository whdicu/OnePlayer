#include "neteasehandler.h"
#include <QCryptographicHash>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>


const static QString FIRST_URL = "http://127.0.0.1:3000";

static NeteaseHandler* netease_handler = nullptr;
NeteaseHandler* NeteaseHandler::getInatance()
{
	if (nullptr == netease_handler)
		netease_handler = new NeteaseHandler;
	return netease_handler;
}

void NeteaseHandler::loginPhone(const QString& phone, const QString& password)
{
	QString passwordMD5 = QCryptographicHash::hash(phone.toUtf8(), QCryptographicHash::Md5).toHex();;
	QString cmd = QString("/login/cellphone?phone=%1&md5_password=%2").arg(phone).arg(passwordMD5);
	auto jo = execPost(cmd);
	dealJsonObject(jo);
}

DSharedPointer<QJsonObject> NeteaseHandler::execPost(const QString& url)
{
	QNetworkRequest request(FIRST_URL + url);
	QNetworkReply* reply = networkManager_->get(request);  // 发送 POST 请求
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
	, networkManager_(new QNetworkAccessManager(this))
{
	loginPhone("15557539750", "Whd2001129");
	
}

NeteaseHandler::~NeteaseHandler()
{
}

void NeteaseHandler::dealJsonObject(DSharedPointer<QJsonObject> obj)
{
	for (auto k1 : obj->keys())
	{
		qDebug() << k1;
		QJsonObject groupObject = obj->value(k1).toObject();
		for (auto k2 : groupObject.keys())
		{
			qDebug() << "\t" << k2;
		}
	}
}
