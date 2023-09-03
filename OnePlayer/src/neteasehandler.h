#pragma once
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QNetworkAccessManager>
#include "DSharedPointer.hpp"

class NeteaseHandler : public QObject
{
	Q_OBJECT

public:
	static NeteaseHandler* getInatance();

	// 手机号登录
	bool loginPhone(const QString& phone, const QString& password);

	// 邮箱登录
	bool loginEmail(const QString& email, const QString& password);


private:
	NeteaseHandler(QObject *parent=nullptr);
	~NeteaseHandler();

	// 阻塞式POST，服务器返回结果后函数才会返回
	DSharedPointer<QJsonObject> execPost(const QString& url, const QString& content);

	void printJsonObject(const QJsonObject& obj, int space=0);


	QNetworkAccessManager* networkManager_;
};
