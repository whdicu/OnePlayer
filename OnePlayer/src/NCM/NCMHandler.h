/*
ncmToMp3.h 对 QT的接口
*/
#pragma once
#include <QImage>
extern "C" {
	#include "ncmToMp3.h"
}

struct CPPMusicData
{
	QByteArray data;
	QString title;
	QString singers;
	QString album;
	QImage image;
	QString format;
};

namespace NCMHandler
{
	CPPMusicData dealNCM(const QString& filename);
};

