#include "PlayerQt.h"
#include "ImageHandler.h"
#include "NCM/NCMHandler.h"
#include "neteasehandler.h"
#include "OneMessageBox.h"
#include "OnePlayerStruct.h"
#include "PlayerFFmpeg.h"
#include <QAudioOutput>
#include <QBuffer>
#include <QDebug>
#include "settinghandler.h"


PlayerQt::PlayerQt(QObject* parent)
	: PlayerBase(parent)
	, player_(new QMediaPlayer)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	, audioOutput_(new QAudioOutput)
#endif
	, startPos_(0)
	, dataBuffer_(nullptr)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	player_->setAudioOutput(audioOutput_);
#endif
	setVolume(SETTING_HANDLER->getStruct().volume);

	connect(player_, &QMediaPlayer::durationChanged, this, [this](qint64 duration)
	{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		// QT6的在playCurrentIndex中设置Pos
		if (startPos_ >= 0)
			setPosition(startPos_);
#endif
		emit durationChanged(duration);
	});
	connect(player_, &QMediaPlayer::positionChanged, this, &PlayerQt::positionChanged);

	connect(player_, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status)
	{
		switch (status)
		{
		//case QMediaPlayer::MediaStatus::BufferedMedia:
		//	slotMetaDataChanged();
		//	break;
		case QMediaPlayer::MediaStatus::EndOfMedia:
			emit mediaAtEnd();
			break;
		}
		
	});
	//connect(player_, &QMediaPlayer::metaDataChanged, this, &PlayerQt::slotMetaDataChanged);

	// 发生错误
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(player_, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error err, const QString& err_str)
	{
		qWarning() << err << "\n" << err_str << "\n";
		switch (err)
		{
		case QMediaPlayer::FormatError:
			break;
		default:
		{
			OneMessageBox::critical(nullptr, tr("发生了意想不到的事情")
				, tr("详情：%1\n文件：%2").arg(err_str).arg(player_->source().fileName()));
			break;
		}
		}
	});
#else
	auto slotError = [this](QMediaPlayer::Error err)
	{
		qWarning() << err << "\n" << player_->errorString() << "\n";
		OneMessageBox::critical(nullptr, tr("发生了意想不到的事情")
			, tr("详情：%1\n文件：%2").arg(player_->errorString())/*.arg(player_->source().fileName())*/);
	};
	connect(player_, SIGNAL(error(QMediaPlayer::Error error)), this, SLOT(slotError));
#endif
}

PlayerQt::~PlayerQt()
{
	player_->stop();
	delete player_;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	delete audioOutput_;
#endif
}

bool PlayerQt::playOrPause()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	if (!player_->hasAudio() && (nullptr == player_->sourceDevice()))
#else
	if (player_->media().isNull() && (nullptr == player_->mediaStream()))
#endif
	{
		playCurrentIndex();
		return true;
	}

	if (QMediaPlayer::PlayingState == GET_PLAY_STATE)
	{
		player_->pause();
		return false;
	}
	else
	{
		player_->play();
		return true;
	}
}

void PlayerQt::playNext()
{
	SETTING_HANDLER->nextMusicIndex();
	playCurrentIndex();
}

void PlayerQt::playPrevious()
{
	SETTING_HANDLER->previousMusicIndex();
	playCurrentIndex();
}

void PlayerQt::setVolume(float vol)
{
	SET_VOLUME(vol);
}

void PlayerQt::setPosition(qint64 pos)
{
	if (pos > player_->duration())
		return;

	player_->setPosition(pos);
}

void PlayerQt::playCurrentIndex(qint64 pos)
{
	emit beginPlay();
	startPos_ = pos;
	QString urlStr = SETTING_HANDLER->currentMusicUrl();

	if (dataBuffer_ != nullptr)
	{
		dataBuffer_->deleteLater();
		dataBuffer_ = nullptr;
	}

	bool ok = false;
	dint64 id = urlStr.toLongLong(&ok);
	if (ok)  // 说明是网易云的音乐的id
	{
		urlStr = NETEASE_HANDLER->getMusicUrl(id);
		NeteaseSongInfo info = SETTING_HANDLER->getNeteaseSongInfo(id);

		ImageDownloadCallBack* callBack = new ImageDownloadCallBack(this);
		connect(callBack, &ImageDownloadCallBack::sigImageSet, this, [this](SharedImage image)
		{
			musicInfo_.image = *image;
			emit MusicInfoChanged(musicInfo_);
		});
		ImageHandler::downloadImage(info.picUrl, callBack);

		musicInfo_.title = info.name;
		musicInfo_.singers = info.singer;
		musicInfo_.album = info.album;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		player_->setSource(QUrl(urlStr));
		setPosition(pos);  // QT5的在durationChanged中设置Pos
#else
		player_->setMedia(QUrl(urlStr));
#endif
		emit MusicInfoChanged(musicInfo_);
	}
	else
	{
		if (urlStr.endsWith(".ncm"))
		{
			ImageDownloadCallBack* callBack = new ImageDownloadCallBack(this);
			connect(callBack, &ImageDownloadCallBack::sigImageSet, this, [this](SharedImage image)
			{
				musicInfo_.image = *image;
				emit MusicInfoChanged(musicInfo_);
			});
			CPPMusicData musicData = NCMHandler::dealNCM(urlStr, callBack);
			//QByteArray aa(reinterpret_cast<const char*>(mb.data.data()), mb.data.size());
			dataBuffer_ = new QBuffer(this);
			dataBuffer_->setData(musicData.data);
			dataBuffer_->open(QIODevice::ReadOnly);

			musicInfo_.title = musicData.title;
			musicInfo_.singers = musicData.singers;
			musicInfo_.album = musicData.album;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			player_->setSourceDevice(dataBuffer_);
			setPosition(pos);  // QT5的在durationChanged中设置Pos
#else
			player_->setMedia(QMediaContent(), dataBuffer_);
#endif
		}
		else
		{
			musicInfo_ = PlayerFFmpeg::analyzeMusicInfo(urlStr);
			if (musicInfo_.image.isNull())
				musicInfo_.image = QImage(":/images/music.png");

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			player_->setSource(QUrl::fromLocalFile(urlStr));
			setPosition(pos);  // QT5的在durationChanged中设置Pos
#else
			player_->setMedia(QUrl::fromLocalFile(urlStr));
#endif
		}
	}
	
	emit MusicInfoChanged(musicInfo_);
	player_->play();
}

void PlayerQt::setLoop(bool loop)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	player_->setLoops(loop ? -1 : 1);
#else
	player_->setPlaybackRate(loop ? QMediaPlaylist::CurrentItemInLoop : QMediaPlaylist::CurrentItemOnce);
#endif
}
