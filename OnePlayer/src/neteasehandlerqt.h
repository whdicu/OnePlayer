#pragma once
#pragma execution_character_set("utf-8")
#include "OnePlayerStruct.h"
#include "apihelper.h"
#include <QObject>


class NeteaseHandlerQT : public QObject
{
	Q_OBJECT

public:
	static NeteaseHandlerQT* getInstance();

	void init() {}
	void uninit() {}

	// 手机号登录
	bool loginPhone(const QString& phone, const QString& password);

	// 发送验证码
	bool sendCaptcha(const QString& phone);

	// 验证码登录
	bool loginCaptcha(const QString& phone, const QString& captcha);

	// 邮箱登录
	bool loginEmail(const QString& email, const QString& password);

	// 检查登录状态
	bool checkLoginStatus();

	bool getUserDetail();

	// 获取该用户的所有歌单信息
	DVector<NeteasePlayListInfo> getAllPlayListsInfo();

	// 获取歌单信息
	NeteasePlayListInfo getPlayListInfo(dint64 id);

	// 获取一个歌单下的所有歌曲
	DVector<NeteaseSongInfo> getSongsfromPlayList(dint64 id);

	// 获取歌曲URL
	QString getMusicUrl(dint64 id);

	// 获取歌词
	QString getLyric(dint64 id);

signals:
	// 下载了新头像
	void sigAvatarImgChanged(const QImage& img);

private:
	NeteaseHandlerQT(QObject *parent=nullptr);
	~NeteaseHandlerQT();

	ApiHelper helper_;
};
