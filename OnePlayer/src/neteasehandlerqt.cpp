#include "neteasehandlerqt.h"
//#include "ImageHandler.h"
//#include "module.h"
#include <QCryptographicHash>
//#include <QDebug>
//#include <QEventLoop>
//#include <QJsonArray>
#include <QJsonObject>
//#include <QJsonParseError>
//#include <QMessageBox>
//#include <QNetworkReply>
//#include "settinghandler.h"
//#include "windows.h"


static NeteaseHandlerQT* netease_handler = nullptr;
NeteaseHandlerQT* NeteaseHandlerQT::getInstance()
{
	if (nullptr == netease_handler)
		netease_handler = new NeteaseHandlerQT;
	return netease_handler;
}

bool NeteaseHandlerQT::loginPhone(const QString& phone, const QString& password)
{
	//qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QString passwordMD5 = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();
	//QString url = QString("/login/cellphone?timestamp=%1").arg(nowTime);  // 加上时间戳防止触发网易云API的缓存机制
	//QString content = QString("phone=%1&md5_password=%2").arg(phone).arg(passwordMD5);
	//DSharedPointer<QJsonObject> jo = execPost(url, content);

	QVariantMap infoMap =
	{
		{"phone", phone},
		{"md5_password", passwordMD5}
	};

	//NeteaseCloudMusicApi api;
	//auto retMap = api.login_cellphone(infoMap);

	/*if (jo->isEmpty())
	{
		QMessageBox::warning(nullptr, tr("登陆失败"), tr("网络太拥挤，请稍后再试"));
		return false;
	}*/

	//int code = jo->value("code").toInt();
	//QString message = jo->value("message").toString();
	//switch (code)
	//{
	//case 502:
	//	QMessageBox::warning(nullptr, tr("登陆失败"), message);
	//	return false;
	//case 200:
	//	QJsonObject accountObj = jo->value("account").toObject();
	//	QJsonObject profileObj = jo->value("profile").toObject();
	//	QString avatarUrl = profileObj.value("avatarUrl").toString();
	//	QString cookie = jo->value("cookie").toString();
	//	break;
	//}
	//qDebug() << *jo;
	return true;
}

bool NeteaseHandlerQT::sendCaptcha(const QString& phone)
{
	//QString url = QString("/captcha/sent");
	//QString content = QString("phone=%1").arg(phone);
	//DSharedPointer<QJsonObject> jo = execPost(url, content);
	//int code = jo->value("code").toInt();
	//return 200 == code;
	return true;
}

bool NeteaseHandlerQT::loginCaptcha(const QString& phone, const QString& captcha)
{
	//qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	//QString url = QString("/login/cellphone?timestamp=%3").arg(nowTime);
	//QString content = QString("phone=%1&captcha=%2").arg(phone).arg(captcha);
	//DSharedPointer<QJsonObject> jo = execPost(url, content);

	//SETTING_HANDLER->getNeteaseInfo().hasLogin = !jo->isEmpty();
	//if (jo->isEmpty())
	//	return false;

	//SETTING_HANDLER->getNeteaseInfo().cookie = jo->value("cookie").toString();
	//SETTING_HANDLER->getNeteaseInfo().token = jo->value("token").toString();
	//QJsonObject profileObj = jo->value("profile").toObject();
	//SETTING_HANDLER->getNeteaseInfo().userId = profileObj.value("userId").toVariant().toLongLong();
	//SETTING_HANDLER->getNeteaseInfo().avatarUrl = profileObj.value("avatarUrl").toString();
	//SETTING_HANDLER->getNeteaseInfo().nickname = profileObj.value("nickname").toString();
	//SETTING_HANDLER->save();

	return true;
}

bool NeteaseHandlerQT::loginEmail(const QString& email, const QString& password)
{
	//qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	//QString passwordMD5 = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();
	//QString url = QString("/login?timestamp=%1").arg(nowTime);  // 加上时间戳防止触发网易云API的缓存机制
	//QString content = QString("email=%1&md5_password=%2").arg(email).arg(passwordMD5);
	//DSharedPointer<QJsonObject> jo = execPost(url, content);

	///*if (jo->isEmpty())
	//{
	//	QMessageBox::warning(nullptr, tr("登陆失败"), tr("网络太拥挤，请稍后再试"));
	//	return false;
	//}*/

	//int code = jo->value("code").toInt();
	//QString message = jo->value("message").toString();
	//switch (code)
	//{
	//case 502:
	//	QMessageBox::warning(nullptr, tr("登陆失败"), message);
	//	return false;
	//}
	//qDebug() << *jo;
	return true;
}

bool NeteaseHandlerQT::checkLoginStatus()
{
	//qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	//QString url = QString("/login/status?timestamp=%1").arg(nowTime);
	//QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	////content += QString("&token=") + SETTING_HANDLER->getNeteaseInfo().token;
	//DSharedPointer<QJsonObject> jo = execPost(url, content);

	//QJsonObject dataObj = jo->value("data").toObject();
	//
	//SETTING_HANDLER->getNeteaseInfo().hasLogin = !dataObj.value("profile").isNull();
	//if (dataObj.value("profile").isNull())
	//	return false;

	//QJsonObject profileObj = dataObj.value("profile").toObject();
	//SETTING_HANDLER->getNeteaseInfo().userId = profileObj.value("userId").toVariant().toLongLong();
	//SETTING_HANDLER->getNeteaseInfo().avatarUrl = profileObj.value("avatarUrl").toString();
	//SETTING_HANDLER->getNeteaseInfo().nickname = profileObj.value("nickname").toString();

	return true;
}

bool NeteaseHandlerQT::getUserDetail()
{
	//QString url = QString("/user/detail");
	//QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	//content += QString("&uid=%1").arg(SETTING_HANDLER->getNeteaseInfo().userId);
	//DSharedPointer<QJsonObject> jo = execPost(url, content);

	return true;
}

DVector<NeteasePlayListInfo> NeteaseHandlerQT::getAllPlayListsInfo()
{
	//QString url = QString("/user/playlist");
	//QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	//content += QString("&uid=%1").arg(SETTING_HANDLER->getNeteaseInfo().userId);
	//DSharedPointer<QJsonObject> jo = execPost(url, content);
	//QJsonArray playlist = jo->value("playlist").toArray();

	DVector<NeteasePlayListInfo> ret;
	//for (const auto& oneList : playlist)
	//{
	//	NeteasePlayListInfo info;
	//	QJsonObject listObj = oneList.toObject();
	//	info.coverImgUrl = listObj.value("coverImgUrl").toString();
	//	info.name = listObj.value("name").toString();
	//	info.id = listObj.value("id").toVariant().toLongLong();
	//	ret.pushBack(info);
	//}

	return ret;
}

NeteasePlayListInfo NeteaseHandlerQT::getPlayListInfo(dint64 id)
{
	//QString url = QString("/playlist/detail");
	//QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	//content += QString("&id=%1").arg(id);
	//DSharedPointer<QJsonObject> jo = execPost(url, content);

	// todo 未完成

	return NeteasePlayListInfo();
}

DVector<NeteaseSongInfo> NeteaseHandlerQT::getSongsfromPlayList(dint64 id)
{
	//QString url = QString("/playlist/track/all");
	//QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	//content += QString("&id=%1").arg(id);
	//DSharedPointer<QJsonObject> jo = execPost(url, content);
	//QJsonArray songs = jo->value("songs").toArray();

	DVector<NeteaseSongInfo> ret;
	//for (const auto& song : songs)
	//{
	//	NeteaseSongInfo info;
	//	QJsonObject songObj = song.toObject();
	//	info.name = songObj.value("name").toString();
	//	info.id = songObj.value("id").toVariant().toLongLong();
	//	info.album = songObj.value("al").toObject().value("name").toString();
	//	info.picUrl = songObj.value("al").toObject().value("picUrl").toString();

	//	info.singer = "";
	//	QJsonArray artistArr = songObj.value("ar").toArray();
	//	for (const auto& artist : artistArr)
	//	{
	//		info.singer.append(artist.toObject().value("name").toString() + ' ');
	//	}
	//	info.singer.trimmed();
	//	
	//	ret.pushBack(info);
	//}

	return ret;
}

QString NeteaseHandlerQT::getMusicUrl(dint64 id)
{
	//QString url = QString("/song/url/v1");
	//QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	//content += QString("&id=%1").arg(id);
	//content += QString("&level=%1").arg("higher");  // 音质 standard higher 
	//DSharedPointer<QJsonObject> jo = execPost(url, content);

	//QJsonArray dataArr = jo->value("data").toArray();
	//if (!dataArr.isEmpty())
	//	return dataArr.first().toObject().value("url").toString();
	//else
		return "";
}

QString NeteaseHandlerQT::getLyric(dint64 id)
{
	//QString url = QString("/lyric");
	//QString content = QString("cookie=") + SETTING_HANDLER->getNeteaseInfo().cookie;
	//content += QString("&id=%1").arg(id);
	//DSharedPointer<QJsonObject> jo = execPost(url, content);

	//if (200 == jo->value("code").toInt())
	//	return jo->value("lrc").toObject().value("lyric").toString();
	//else
		return "";
}

DSharedPointer<QJsonObject> NeteaseHandlerQT::execPost(const QString& url, const QString& content)
{
	//QNetworkRequest request(FIRST_URL + url);
	//QNetworkReply* reply = networkManager_->post(request, content.toUtf8());  // 发送 POST 请求
	DSharedPointer<QJsonObject> ret(new QJsonObject);

	//QEventLoop loop;
	//connect(reply, &QNetworkReply::finished, this, [reply, &ret, &loop]()
	//{
	//	if (reply->error() == QNetworkReply::NoError)
	//	{
	//		QByteArray responseData = reply->readAll();
	//		qDebug() << responseData.size();
	//		QString ss = responseData;
	//		reply->deleteLater();
	//		QJsonParseError parseError;
	//		QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &parseError);
	//		if (QJsonParseError::NoError != parseError.error)
	//		{
	//			qDebug() << "Json file analyze failed" << __FUNCTION__;
	//		}
	//		else
	//		{
	//			*ret = jsonDoc.object();
	//		}
	//	}
	//	else
	//	{
	//		// 处理错误情况
	//		reply->deleteLater();
	//		qWarning() << "errType:" << reply->error() << "errStr:" << reply->errorString();
	//	}
	//	loop.quit();
	//});
	//loop.exec();

	return ret;
}

NeteaseHandlerQT::NeteaseHandlerQT(QObject *parent)
	: QObject(parent)
	//, apiProcess_(new QProcess(this))
	//, networkManager_(new QNetworkAccessManager(this))
{

}

NeteaseHandlerQT::~NeteaseHandlerQT()
{
	
}

void NeteaseHandlerQT::printJsonObject(const QJsonObject& obj, int space)
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
