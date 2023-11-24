#include "PlayerQt.h"
#include "OnePlayerStruct.h"
#include <QDebug>
#include <QMediaMetaData>
#include "settinghandler.h"


PlayerQt::PlayerQt(QObject* parent)
	: PlayerBase(parent)
	, player_(new QMediaPlayer)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	, audioOutput_(new QAudioOutput(this))
#endif
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	player_->setAudioOutput(audioOutput_);
#endif
	setVolume(SETTING_HANDLER->getStruct().volume);

	connect(player_, &QMediaPlayer::durationChanged, this, &PlayerQt::durationChanged);
	connect(player_, &QMediaPlayer::positionChanged, this, &PlayerQt::positionChanged);
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//	connect(player_, &QMediaPlayer::sourceChanged, this, &PlayerQt::sourceChanged);
//#else
//	connect(player_, &QMediaPlayer::mediaChanged, this, [this](const QMediaContent& media)
//	{
//		//qDebug() << media.canonicalUrl().fileName();
//		//qDebug() << media.canonicalUrl().path();
//		//qDebug() << media.canonicalUrl().toString();
//		//qDebug() << media.canonicalUrl().toLocalFile();
//		emit sourceChanged(media.canonicalUrl());
//	});
//#endif
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
}

PlayerQt::~PlayerQt()
{
	player_->stop();
}

bool PlayerQt::playOrPause()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	if (!player_->hasAudio())
#else
	if (player_->media().isNull())
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
	player_->setPosition(pos);
}

void PlayerQt::playCurrentIndex(qint64 pos)
{
	emit beginPlay();
	QUrl url = SETTING_HANDLER->currentMusicUrl();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	player_->setSource(url);
#else
	player_->setMedia(url);
#endif
	emit sourceChanged(url);
	setPosition(pos);
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
