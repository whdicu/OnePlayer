#pragma once
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QProcess>
#include <QNetworkAccessManager>
#include "HDMemory/DSharedPointer.hpp"


class NeteaseHandler : public QObject
{
	Q_OBJECT

public:
	static NeteaseHandler* getInstance();

	// 启动网易云API的exe程序
	void startApiExe();
	void stopApiExe();

	// 手机号登录
	bool loginPhone(const QString& phone, const QString& password);

	// 发送验证码
	bool sendCaptcha(const QString& phone);

	// 验证码登录
	bool loginCaptcha(const QString& phone, const QString& captcha);

	// 邮箱登录
	bool loginEmail(const QString& email, const QString& password);

	// 检查登录状态
	int checkLoginStatus();

signals:
	// 下载了新头像
	void sigAvatarImgChanged(const QImage& img);

private:
	NeteaseHandler(QObject *parent=nullptr);
	~NeteaseHandler();

	// 阻塞式POST，服务器返回结果后函数才会返回
	DSharedPointer<QJsonObject> execPost(const QString& url, const QString& content);

	void printJsonObject(const QJsonObject& obj, int space=0);

	QProcess* apiProcess_;
	QNetworkAccessManager* networkManager_;
};

#ifndef NETEASE_HANDLER
#define NETEASE_HANDLER NeteaseHandler::getInstance()
#endif
