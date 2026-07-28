/*
ncmToMp3.h 对 QT的接口
*/
#pragma once
#include <QImage>
#include "ncmToMp3.h"

class ImageDownloadCallBack;

struct CPPMusicInfo
{
	QByteArray data;
	QString title;
	QString singers;
	QString album;
	QImage image;
	QString format;
	qint64 musicId;
};

namespace NCMHandler
{
	CPPMusicInfo dealNCM(const QString& filename, ImageDownloadCallBack* callBack = nullptr);
};

