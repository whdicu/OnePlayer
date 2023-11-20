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

QImage PlayerFFmpeg::getMusicImage(const QString& musicPath)
{
	QImage ret = QImage();
	AVFormatContext* fmt_ctx = nullptr;
	if (avformat_open_input(&fmt_ctx, musicPath.toUtf8(), nullptr, nullptr))
	{
		qDebug() << "Fail to open file";
		return ret;
	}

	//读取metadata中所有的tag
	//while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
	//{
	//	qDebug() << tag->key << tag->value;
	//}

	// read the format headers
	//if (fmt_ctx->iformat->read_header(fmt_ctx) < 0)
	//{
	//	qDebug() << "No header format";
	//	return QImage();
	//}

	for (int i = 0; i < fmt_ctx->nb_streams; i++)
	{
		if (fmt_ctx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC)
		{
			AVPacket pkt = fmt_ctx->streams[i]->attached_pic;
			ret = QImage::fromData((uchar*)pkt.data, pkt.size);
			break;
		}
	}

	avformat_close_input(&fmt_ctx);
	avformat_free_context(fmt_ctx);

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

void PlayerFFmpeg::slotMetaDataChanged()
{
	
}
