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

	// 手机号登录，使用密码
	bool loginPhonePassword(const QString& phone, const QString& password, QString& errMsg);

	// 手机号登录，使用验证码
	bool loginPhoneCaptcha(const QString& phone, const QString& captcha, QString& errMsg);

	// 发送验证码
	bool sendCaptcha(const QString& phone);

	// 验证码登录
	bool checkCaptcha(const QString& phone, const QString& captcha, QString& errMsg);

	// 刷新登录，会返回新的cookie（二维码登录除外）
	bool loginRefresh();

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

	// 获取歌曲红心数量
	qint64 getSongRedCount(dint64 id);

	// 搜索
	DVector<NeteaseSongInfo> search(const QString& keywords, int limit=30, int type=1);

	// 获取歌手全部歌曲，order：hot 按热门排序    time 按时间排序
	DVector<NeteaseSongInfo> getArtistSongs(dint64 id, const QString& order="hot", int limit=50, int offset=0);

	// 获取曲风列表
	QVariantMap getStyleList();

	// 获取曲风对应歌曲
	DVector<NeteaseSongInfo> getStyleSongs(dint64 tagId, int size=20, qint64 cursor=0, int sort=0);

	// 二维码登录
	QString getQrKey();
	QString getQrImageUrl(const QString& key);
	int checkQrStatus(const QString& key);

signals:
	// 下载了新头像
	void sigAvatarImgChanged(const QImage& img);

private:
	NeteaseHandlerQT(QObject *parent=nullptr);
	~NeteaseHandlerQT();

	ApiHelper helper_;
};
