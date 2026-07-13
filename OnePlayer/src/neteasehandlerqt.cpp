#include "neteasehandlerqt.h"
#include "ImageHandler.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QNetworkReply>
#include "settinghandler.h"


static NeteaseHandlerQT* netease_handler = nullptr;
NeteaseHandlerQT* NeteaseHandlerQT::getInstance()
{
	if (nullptr == netease_handler)
		netease_handler = new NeteaseHandlerQT;
	return netease_handler;
}

bool NeteaseHandlerQT::loginPhone(const QString& phone, const QString& password)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QString passwordMD5 = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();

	QVariantMap retMap = helper_.invoke("login_cellphone", {
		{ "phone", phone },
		{ "md5_password", passwordMD5 },
		{ "timestamp", nowTime }
	});

	int code = retMap.value("code").toInt();
	QString message = retMap.value("message").toString();
	switch (code)
	{
	case 502:
		QMessageBox::warning(nullptr, tr("登陆失败"), message);
		return false;
	case 200:
	{
		QVariantMap accountObj = retMap.value("account").toMap();
		QVariantMap profileObj = retMap.value("profile").toMap();
		QString avatarUrl = profileObj.value("avatarUrl").toString();
		QString cookie = retMap.value("cookie").toString();
		break;
	}
	}
	qDebug() << retMap;
	return true;
}

bool NeteaseHandlerQT::sendCaptcha(const QString& phone)
{
	QVariantMap retMap = helper_.invoke("captcha_sent", {
		{ "cellphone", phone }
	});
	int code = retMap.value("code").toInt();
	return 200 == code;
}

bool NeteaseHandlerQT::loginCaptcha(const QString& phone, const QString& captcha)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QVariantMap retMap = helper_.invoke("login_cellphone", {
		{ "phone", phone },
		{ "captcha", captcha },
		{ "timestamp", nowTime }
	});

	SETTING_HANDLER->getNeteaseInfo().hasLogin = !retMap.isEmpty();
	if (retMap.isEmpty())
		return false;

	SETTING_HANDLER->getNeteaseInfo().cookie = retMap.value("cookie").toString();
	SETTING_HANDLER->getNeteaseInfo().token = retMap.value("token").toString();
	QVariantMap profileObj = retMap.value("profile").toMap();
	SETTING_HANDLER->getNeteaseInfo().userId = profileObj.value("userId").toLongLong();
	SETTING_HANDLER->getNeteaseInfo().avatarUrl = profileObj.value("avatarUrl").toString();
	SETTING_HANDLER->getNeteaseInfo().nickname = profileObj.value("nickname").toString();
	SETTING_HANDLER->save();

	return true;
}

bool NeteaseHandlerQT::loginEmail(const QString& email, const QString& password)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QString passwordMD5 = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();

	QVariantMap retMap = helper_.invoke("login", {
		{ "email", email },
		{ "md5_password", passwordMD5 },
		{ "timestamp", nowTime }
	});

	int code = retMap.value("code").toInt();
	QString message = retMap.value("message").toString();
	switch (code)
	{
	case 502:
		QMessageBox::warning(nullptr, tr("登陆失败"), message);
		return false;
	}
	qDebug() << retMap;
	return true;
}

bool NeteaseHandlerQT::checkLoginStatus()
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();

	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("login_status", {
		{ "timestamp", nowTime }
	});

	QVariantMap dataObj = retMap.value("data").toMap();

	SETTING_HANDLER->getNeteaseInfo().hasLogin = !dataObj.value("profile").isNull();
	if (dataObj.value("profile").isNull())
		return false;

	QVariantMap profileObj = dataObj.value("profile").toMap();
	SETTING_HANDLER->getNeteaseInfo().userId = profileObj.value("userId").toLongLong();
	SETTING_HANDLER->getNeteaseInfo().avatarUrl = profileObj.value("avatarUrl").toString();
	SETTING_HANDLER->getNeteaseInfo().nickname = profileObj.value("nickname").toString();

	return true;
}

bool NeteaseHandlerQT::getUserDetail()
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("user_detail", {
		{ "uid", SETTING_HANDLER->getNeteaseInfo().userId }
	});

	return true;
}

DVector<NeteasePlayListInfo> NeteaseHandlerQT::getAllPlayListsInfo()
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("user_playlist", {
		{ "uid", SETTING_HANDLER->getNeteaseInfo().userId }
	});
	QVariantList playlist = retMap.value("playlist").toList();

	DVector<NeteasePlayListInfo> ret;
	for (const auto& oneList : playlist)
	{
		NeteasePlayListInfo info;
		QVariantMap listObj = oneList.toMap();
		info.coverImgUrl = listObj.value("coverImgUrl").toString();
		info.name = listObj.value("name").toString();
		info.id = listObj.value("id").toLongLong();
		ret.pushBack(info);
	}

	return ret;
}

NeteasePlayListInfo NeteaseHandlerQT::getPlayListInfo(dint64 id)
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("playlist_detail", {
		{ "id", id }
	});

	// todo 未完成

	return NeteasePlayListInfo();
}

DVector<NeteaseSongInfo> NeteaseHandlerQT::getSongsfromPlayList(dint64 id)
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("playlist_track_all", {
		{ "id", id }
	});
	QVariantList songs = retMap.value("songs").toList();

	DVector<NeteaseSongInfo> ret;
	for (const auto& song : songs)
	{
		NeteaseSongInfo info;
		QVariantMap songObj = song.toMap();
		info.name = songObj.value("name").toString();
		info.id = songObj.value("id").toLongLong();
		info.album = songObj.value("al").toMap().value("name").toString();
		info.picUrl = songObj.value("al").toMap().value("picUrl").toString();

		info.singer = "";
		QVariantList artistArr = songObj.value("ar").toList();
		for (const auto& artist : artistArr)
		{
			info.singer.append(artist.toMap().value("name").toString() + ' ');
		}
		info.singer = info.singer.trimmed();

		ret.pushBack(info);
	}

	return ret;
}

QString NeteaseHandlerQT::getMusicUrl(dint64 id)
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("song_url_v1", {
		{ "id", id },
		{ "level", "higher" }  // 音质 standard higher
	});

	QVariantList dataArr = retMap.value("data").toList();
	if (!dataArr.isEmpty())
		return dataArr.first().toMap().value("url").toString();
	else
		return "";
}

QString NeteaseHandlerQT::getLyric(dint64 id)
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("lyric_new", {
		{ "id", id }
	});

	if (200 == retMap.value("code").toInt())
		return retMap.value("lrc").toMap().value("lyric").toString();
	else
		return "";
}

NeteaseHandlerQT::NeteaseHandlerQT(QObject *parent)
	: QObject(parent)
	, helper_(this)
{

}

NeteaseHandlerQT::~NeteaseHandlerQT()
{

}
