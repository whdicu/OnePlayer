#include "NCMHandler.h"
#include "ImageHandler.h"
#include "OnePlayerStruct.h"
#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}


CPPMusicInfo NCMHandler::dealNCM(const QString& filename, ImageDownloadCallBack* callBack)
{
	CPPMusicInfo ret;
	NCMMusicInfo musicData;
	int status = getFileData(filename, &musicData);
	ret.data = musicData.data;
	ret.image.loadFromData(musicData.imgData);

	QJsonObject obj = musicData.json;
	if (obj.isEmpty())
	{
		qWarning() << __FUNCTION__ << "JsonStr analyze failed";
		return ret;
	}
	/*
	
	{
		"musicId":	"track-108914",
		"musicName":	"江南",
		"artist":	[["林俊杰", "3684"]],
		"albumId":	"10804",
		"album":	"第二天堂",
		"albumPicDocId":	"109951168111265583",
		"albumPic":	"http://p3.music.126.net/_0OAhWhIbg-nOP-6e4o-SA==/109951168111265583.jpg",
		"bitrate":	128000,
		"mp3DocId":	"811c35a7f7f552989edcaea92871b544",
		"duration":	267946,
		"mvId":	"522362",
		"alias":	[],
		"transNames":	["River South"],
		"format":	"mp3",
		"fee":	1,
		"privilege":	{
			"flag":	1278212
		}
	}
	
	*/

	ret.title = obj["musicName"].toString();
	QJsonArray artists = obj["artist"].toArray().first().toArray();
	//QString artistsStr;
	//for (auto artist : artists)
	//	artistsStr.append(artist.toString()).append(' ');
	//ret.singers = artistsStr.trimmed();
	ret.singers = artists.isEmpty() ? "" : artists.first().toString();
	ret.album = obj["album"].toString();
	ret.format = obj["format"].toString();
	ret.musicId = obj["musicId"].toVariant().toLongLong();

	// 现在图片直接从ncm文件中获取，不再从网易云下载
	//if (obj.contains("albumPic"))
	//{
	//	QString url = obj["albumPic"].toString()
	//		+ QString("?param=%1y%2").arg(MUSIC_INFO_WIDGET_WIDTH).arg(MUSIC_INFO_WIDGET_HEIGHT);
	//	
	//	ImageHandler::downloadImage(url, callBack);
	//}

	//if (!albumImg.isNull())
	//{
 //       //addImage2Data(musicData, albumImg, musicData);
 //       //func1(musicData);
 //       //addAlbumCover(albumImg);

	//	//qint64 i1 = QDateTime::currentMSecsSinceEpoch();
	//	//albumImg.save("C:\\Users\\WHDon\\Desktop\\asdas.jpg");
	//	//qDebug() << (QDateTime::currentMSecsSinceEpoch() - i1);
	//}

	//QFile file(outputDir + "/asd.mp3");
	//file.open(QIODevice::WriteOnly);
	//file.write(musicData);
	//file.close();


	//QVariantMap map = obj.toVariantMap();
	//for (auto it = map.begin(); it != map.end(); ++it)
	//	qDebug() << it.key() << it.value();

	return ret;
}
