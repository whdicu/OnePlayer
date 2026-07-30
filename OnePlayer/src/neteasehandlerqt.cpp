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

bool NeteaseHandlerQT::loginPhonePassword(const QString& phone, const QString& password, QString& errMsg)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QString passwordMD5 = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();

	QVariantMap retMap = helper_.invoke("login_cellphone", {
		{ "phone", phone },
		{ "md5_password", passwordMD5 },
		{ "timestamp", nowTime }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();
	int code = bodyMap.value("code").toInt();
	errMsg = bodyMap.value("message").toString();
	if (code == 200)
	{
		QVariantMap accountObj = bodyMap.value("account").toMap();
		QVariantMap profileObj = bodyMap.value("profile").toMap();
		QString avatarUrl = profileObj.value("avatarUrl").toString();
		QString cookie = bodyMap.value("cookie").toString();
	}

	return code == 200;
	return true;
}

bool NeteaseHandlerQT::loginPhoneCaptcha(const QString& phone, const QString& captcha, QString& errMsg)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();

	QVariantMap retMap = helper_.invoke("login_cellphone", {
		{ "phone", phone },
		{ "captcha", captcha },
		{ "timestamp", nowTime }
		});

	QVariantMap bodyMap = retMap.value("body").toMap();
	int code = bodyMap.value("code").toInt();
	errMsg = bodyMap.value("message").toString();
	if (code == 200)
	{
		QVariantMap accountObj = bodyMap.value("account").toMap();
		QVariantMap profileObj = bodyMap.value("profile").toMap();
		QString avatarUrl = profileObj.value("avatarUrl").toString();
		QString cookie = bodyMap.value("cookie").toString();
	}
	
	return code == 200;
}

bool NeteaseHandlerQT::sendCaptcha(const QString& phone)
{
	QVariantMap retMap = helper_.invoke("captcha_sent", {
		{ "cellphone", phone }
	});
	QVariantMap bodyMap = retMap.value("body").toMap();
	int code = bodyMap.value("code").toInt();
	return 200 == code;
}

bool NeteaseHandlerQT::checkCaptcha(const QString& phone, const QString& captcha, QString& errMsg)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
	QVariantMap retMap = helper_.invoke("captcha_verify", {
		{ "phone", phone },
		{ "captcha", captcha },
		{ "timestamp", nowTime }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();
	errMsg = bodyMap.value("message").toString();
	bool ret = bodyMap.value("data").toBool();
	SETTING_HANDLER->getNeteaseInfo().hasLogin = ret;

	return ret;

	//SETTING_HANDLER->getNeteaseInfo().hasLogin = !retMap.isEmpty();
	//if (retMap.isEmpty())
	//	return false;

	//SETTING_HANDLER->getNeteaseInfo().cookie = retMap.value("cookie").toString();
	//SETTING_HANDLER->getNeteaseInfo().token = retMap.value("token").toString();
	//QVariantMap profileObj = retMap.value("profile").toMap();
	//SETTING_HANDLER->getNeteaseInfo().userId = profileObj.value("userId").toLongLong();
	//SETTING_HANDLER->getNeteaseInfo().avatarUrl = profileObj.value("avatarUrl").toString();
	//SETTING_HANDLER->getNeteaseInfo().nickname = profileObj.value("nickname").toString();
	//SETTING_HANDLER->save();

	return true;
}

bool NeteaseHandlerQT::loginRefresh()
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();

	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("login_refresh", {
		{ "timestamp", nowTime }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();
	SETTING_HANDLER->getNeteaseInfo().cookie = retMap.value("cookie").toString();
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

	QVariantMap bodyMap = retMap.value("body").toMap();
	QVariantMap dataObj = bodyMap.value("data").toMap();

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
	QVariantMap bodyMap = retMap.value("body").toMap();
	QVariantList playlist = bodyMap.value("playlist").toList();

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
	QVariantMap bodyMap = retMap.value("body").toMap();
	QVariantList songs = bodyMap.value("songs").toList();

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

	QVariantMap bodyMap = retMap.value("body").toMap();
	QVariantList dataArr = bodyMap.value("data").toList();
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

	QVariantMap bodyMap = retMap.value("body").toMap();

	if (200 == bodyMap.value("code").toInt())
		return bodyMap.value("lrc").toMap().value("lyric").toString();
	else
		return "";
}

qint64 NeteaseHandlerQT::getSongRedCount(dint64 id)
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("song_red_count", {
		{ "id", id }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();

	if (200 == bodyMap.value("code").toInt())
		return bodyMap.value("data").toMap().value("count").toLongLong();
	else
		return 0;
}

DVector<NeteaseSongInfo> NeteaseHandlerQT::search(const QString& keywords, int limit, int type)
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("cloudsearch", {
		{ "keywords", keywords },
		{ "limit", limit },
		{ "type", type }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();
	//qDebug() << bodyMap;
	QVariantMap resultObj = bodyMap.value("result").toMap();
	QVariantList songs = resultObj.value("songs").toList();

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

DVector<NeteaseSongInfo> NeteaseHandlerQT::getArtistSongs(dint64 id, const QString& order, int limit, int offset)
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("artist_songs", {
		{ "id", id },
		{ "order", order },
		{ "limit", limit },
		{ "offset", offset }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();
	QVariantList songs = bodyMap.value("songs").toList();

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

QVariantMap NeteaseHandlerQT::getStyleList()
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("style_list", {});

	return retMap.value("body").toMap();
}

DVector<NeteaseSongInfo> NeteaseHandlerQT::getStyleSongs(dint64 tagId, int size, qint64 cursor, int sort)
{
	// 设置 cookie
	helper_.set_cookie(SETTING_HANDLER->getNeteaseInfo().cookie);

	QVariantMap retMap = helper_.invoke("style_song", {
		{ "tagId", tagId },
		{ "size", size },
		{ "cursor", cursor },
		{ "sort", sort }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();
	QVariantList songs = bodyMap.value("data").toMap().value("songs").toList();

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

QString NeteaseHandlerQT::getQrKey()
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();

	QVariantMap retMap = helper_.invoke("login_qr_key", {
		{ "timestamp", nowTime }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();
	if (200 == bodyMap.value("code").toInt())
		return bodyMap.value("data").toMap().value("unikey").toString();
	else
		return "";
}

QString NeteaseHandlerQT::getQrImageUrl(const QString& key)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();

	QVariantMap retMap = helper_.invoke("login_qr_create", {
		{ "key", key },
		{ "qrimg", true },
		{ "timestamp", nowTime }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();
	if (200 == bodyMap.value("code").toInt())
		return bodyMap.value("data").toMap().value("qrurl").toString();
	else
		return "";
}

int NeteaseHandlerQT::checkQrStatus(const QString& key)
{
	qint64 nowTime = QDateTime::currentMSecsSinceEpoch();

	QVariantMap retMap = helper_.invoke("login_qr_check", {
		{ "key", key },
		{ "timestamp", nowTime }
	});

	QVariantMap bodyMap = retMap.value("body").toMap();
	if (200 == bodyMap.value("code").toInt())
	{
		// 803: 授权成功，保存 cookie
		QString cookie = bodyMap.value("cookie").toString();
		if (!cookie.isEmpty())
		{
			SETTING_HANDLER->getNeteaseInfo().cookie = cookie;
			SETTING_HANDLER->save();
		}
	}
	return bodyMap.value("code").toInt();
}

NeteaseHandlerQT::NeteaseHandlerQT(QObject *parent)
	: QObject(parent)
	, helper_(this)
{

}

NeteaseHandlerQT::~NeteaseHandlerQT()
{

}
