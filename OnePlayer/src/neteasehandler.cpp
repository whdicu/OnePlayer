#include "neteasehandler.h"
#include "ImageHandler.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QNetworkReply>
#include "settinghandler.h"
#include "windows.h"


const static QString FIRST_URL = "http://127.0.0.1:3000";

static NeteaseHandler* netease_handler = nullptr;
NeteaseHandler* NeteaseHandler::getInstance()
{
	if (nullptr == netease_handler)
		netease_handler = new NeteaseHandler;
	return netease_handler;
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
	case 200:
		QJsonObject accountObj = jo->value("account").toObject();
		QJsonObject profileObj = jo->value("profile").toObject();
		QString avatarUrl = profileObj.value("avatarUrl").toString();
		QString cookie = jo->value("cookie").toString();
		break;
	}
	qDebug() << *jo;
	return true;
}

bool NeteaseHandler::sendCaptcha(const QString& phone)
{
	QString url = QString("/captcha/sent");
	QString content = QString("phone=%1").arg(phone);
	DSharedPointer<QJsonObject> jo = execPost(url, content);
	int code = jo->value("code").toInt();
	return 200 == code;
}

bool NeteaseHandler::loginCaptcha(const QString& phone, const QString& captcha)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QString url = QString("/login/cellphone?timestamp=%3").arg(nowTime);
	QString content = QString("phone=%1&captcha=%2").arg(phone).arg(captcha);
	DSharedPointer<QJsonObject> jo = execPost(url, content);

	SETTING_HANDLER->getNeteaseInfo().hasLogin = !jo->isEmpty();
	if (jo->isEmpty())
		return false;

	SETTING_HANDLER->getNeteaseInfo().cookie = jo->value("cookie").toString();
	SETTING_HANDLER->getNeteaseInfo().token = jo->value("token").toString();
	QJsonObject profileObj = jo->value("profile").toObject();
	SETTING_HANDLER->getNeteaseInfo().userId = profileObj.value("userId").toVariant().toLongLong();
	SETTING_HANDLER->getNeteaseInfo().avatarUrl = profileObj.value("avatarUrl").toString();
	SETTING_HANDLER->getNeteaseInfo().nickname = profileObj.value("nickname").toString();
	SETTING_HANDLER->save();

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

bool NeteaseHandler::checkLoginStatus()
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QString url = QString("/login/status?timestamp=%1").arg(nowTime);
	QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	//content += QString("&token=") + SETTING_HANDLER->getNeteaseInfo().token;
	DSharedPointer<QJsonObject> jo = execPost(url, content);

	QJsonObject dataObj = jo->value("data").toObject();
	
	SETTING_HANDLER->getNeteaseInfo().hasLogin = !dataObj.value("profile").isNull();
	if (dataObj.value("profile").isNull())
		return false;

	QJsonObject profileObj = dataObj.value("profile").toObject();
	SETTING_HANDLER->getNeteaseInfo().userId = profileObj.value("userId").toVariant().toLongLong();
	SETTING_HANDLER->getNeteaseInfo().avatarUrl = profileObj.value("avatarUrl").toString();
	SETTING_HANDLER->getNeteaseInfo().nickname = profileObj.value("nickname").toString();

	return true;
}

bool NeteaseHandler::getUserDetail()
{
	QString url = QString("/user/detail");
	QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	content += QString("&uid=%1").arg(SETTING_HANDLER->getNeteaseInfo().userId);
	DSharedPointer<QJsonObject> jo = execPost(url, content);

	return true;
}

DVector<NeteasePlayListInfo> NeteaseHandler::getPlayLists()
{
	QString url = QString("/user/playlist");
	QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	content += QString("&uid=%1").arg(SETTING_HANDLER->getNeteaseInfo().userId);
	DSharedPointer<QJsonObject> jo = execPost(url, content);
	QJsonArray playlist = jo->value("playlist").toArray();

	DVector<NeteasePlayListInfo> ret;
	for (const auto& oneList : playlist)
	{
		NeteasePlayListInfo info;
		QJsonObject listObj = oneList.toObject();
		info.coverImgUrl = listObj.value("coverImgUrl").toString();
		info.name = listObj.value("name").toString();
		info.id = listObj.value("id").toVariant().toLongLong();
		ret.pushBack(info);
	}

	return ret;
}

DVector<NeteaseSongInfo> NeteaseHandler::getSongsfromPlayList(qint64 id)
{
	QString url = QString("/playlist/track/all");
	QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	content += QString("&id=%1").arg(id);
	DSharedPointer<QJsonObject> jo = execPost(url, content);
	QJsonArray songs = jo->value("songs").toArray();

	DVector<NeteaseSongInfo> ret;
	for (const auto& song : songs)
	{
		NeteaseSongInfo info;
		QJsonObject songObj = song.toObject();
		info.name = songObj.value("name").toString();
		info.id = songObj.value("id").toVariant().toLongLong();
		ret.pushBack(info);
	}

	return ret;
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
			//qDebug() << responseData.size();
			//QString ss = responseData;
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
			qWarning() << "errType:" << reply->error() << "errStr:" << reply->errorString();
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

}

NeteaseHandler::~NeteaseHandler()
{
	
}

void NeteaseHandler::startApiExe()
{
	//apiThread_->start();
	apiProcess_->start("NeteaseCloudMusicApi.exe");
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
