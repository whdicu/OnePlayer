#include "PlayerFFmpeg.h"
//#include "OnePlayerStruct.h"
#include <QDebug>
#include "settinghandler.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
}

MusicInfo PlayerFFmpeg::analyzeMusicInfo(const QString& musicPath)
{
	MusicInfo ret = MusicInfo();
	AVFormatContext* formatContext = nullptr;
	if (avformat_open_input(&formatContext, musicPath.toUtf8(), nullptr, nullptr))
	{
		qDebug() << "Fail to open file";
		return ret;
	}

	//读取metadata中所有的tag
	AVDictionaryEntry* tag = nullptr;
	if (tag = av_dict_get(formatContext->metadata, "title", nullptr, AV_DICT_IGNORE_SUFFIX))
		ret.title = tag->value;
	if (tag = av_dict_get(formatContext->metadata, "artist", nullptr, AV_DICT_IGNORE_SUFFIX))
		ret.singers = tag->value;
	if (tag = av_dict_get(formatContext->metadata, "album", nullptr, AV_DICT_IGNORE_SUFFIX))
		ret.album = tag->value;
	//AVDictionaryEntry* tag = nullptr;
	//while ((tag = av_dict_get(formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
	//{
	//	qDebug() << tag->key << tag->value;
	//}

	// read the format headers
	//if (formatContext->iformat->read_header(formatContext) < 0)
	//{
	//	qDebug() << "No header format";
	//	return QImage();
	//}

	for (int i = 0; i < formatContext->nb_streams; i++)
	{
		if (formatContext->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC)
		{
			AVPacket pkt = formatContext->streams[i]->attached_pic;
			ret.image = QImage::fromData((uchar*)pkt.data, pkt.size);
			break;
		}
	}

	avformat_close_input(&formatContext);
	avformat_free_context(formatContext);

	return ret;
}

PlayerFFmpeg::PlayerFFmpeg(QObject* parent)
	: PlayerBase(parent)
{
	qDebug() << avcodec_configuration();
}

PlayerFFmpeg::~PlayerFFmpeg()
{}

bool PlayerFFmpeg::playOrPause()
{
	return true;
}

void PlayerFFmpeg::stop()
{
	
}

void PlayerFFmpeg::playNext()
{
	
}

void PlayerFFmpeg::playPrevious()
{
	
}

void PlayerFFmpeg::setVolume(float vol)
{
	
}

void PlayerFFmpeg::setPosition(qint64 pos)
{
	
}
