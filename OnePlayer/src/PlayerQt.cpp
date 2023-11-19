#include "PlayerQt.h"
#include "OnePlayerStruct.h"
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
	connect(player_, &QMediaPlayer::sourceChanged, this, &PlayerQt::sourceChanged);
	connect(player_, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status)
	{
		switch (status)
		{
		case QMediaPlayer::MediaStatus::BufferedMedia:
			slotMetaDataChanged();
			break;
		case QMediaPlayer::MediaStatus::EndOfMedia:
			emit mediaAtEnd();
			break;
		}
		
	});
	//connect(player_, &QMediaPlayer::metaDataChanged, this, &PlayerQt::slotMetaDataChanged);
}

PlayerQt::~PlayerQt()
{}

bool PlayerQt::playOrPause()
{
	if (!player_->hasAudio())
	{
		player_->setSource(SETTING_HANDLER->nowMusicPath());
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
	QString musicPath = SETTING_HANDLER->nextMusicPath();
	play(musicPath);
}

void PlayerQt::playPrevious()
{
	QString musicPath = SETTING_HANDLER->previousMusicPath();
	play(musicPath);
}

void PlayerQt::setVolume(float vol)
{
	SET_VOLUME(vol);
}

void PlayerQt::setPosition(qint64 pos)
{
	player_->setPosition(pos);
}

void PlayerQt::play(const QString& musicPath)
{
	emit beginPlay();
	player_->setSource(musicPath);
	player_->play();
}

void PlayerQt::slotMetaDataChanged()
{
	MusicMetaData metaData;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define GET_META_DATA player_->metaData().value
#else
#define GET_META_DATA player_->metaData
#endif
	auto ss = player_->metaData();
	qDebug() << GET_META_DATA(QMediaMetaData::MediaType).toString();
	qDebug() << GET_META_DATA(QMediaMetaData::AudioCodec).toInt();

	metaData.title = GET_META_DATA(QMediaMetaData::Title).toString();
	QStringList authorList = GET_META_DATA(QMediaMetaData::Author).toStringList();  // 去重
	//QImage thumbnailImage = GET_META_DATA(QMediaMetaData::ThumbnailImage).value<QImage>();
	//qDebug() << thumbnailImage.width() << thumbnailImage.height();
	//QImage coverArtImage = GET_META_DATA(QMediaMetaData::CoverArtImage).value<QImage>();
	//qDebug() << coverArtImage.width() << coverArtImage.height();
	metaData.albumTitle = GET_META_DATA(QMediaMetaData::AlbumTitle).toString();
	//metaData.albumArtist = GET_META_DATA(QMediaMetaData::AlbumArtist).toStringList();
	//        QString genre = GET_META_DATA(QMediaMetaData::Genre).toString();  // 流派
	//        QStringList contributing_artist = GET_META_DATA(QMediaMetaData::ContributingArtist).toStringList();  // 去重 贡献艺术家

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QSet<QString> authorSet(authorList.begin(), authorList.end());
#else
	QSet<QString> authorSet = authorList.toSet();
#endif
	QString temp = "";
	for (const QString& singer : authorSet)
	{
		temp.append(singer).append(' ');
	}
	metaData.singers = temp.trimmed();

	//if (thumbnailImage.isNull())
	//	metaData.image = coverArtImage;
	//	//refreshImageWidget(cover_art_image, title, singers, album_title);
	//else
	//	metaData.image = thumbnailImage;
	//	//refreshImageWidget(thumbnail_image, title, singers, album_title);


	//        qDebug() << "Comment" << meta_data.value(QMediaMetaData::Comment);
	//        qDebug() << "Description" << meta_data.value(QMediaMetaData::Description);
	//        qDebug() << "Date" << meta_data.value(QMediaMetaData::Date);
	//        qDebug() << "Language" << meta_data.value(QMediaMetaData::Language);
	//        qDebug() << "Publisher" << meta_data.value(QMediaMetaData::Publisher);
	//        qDebug() << "Copyright" << meta_data.value(QMediaMetaData::Copyright);
	//        qDebug() << "Duration" << meta_data.value(QMediaMetaData::Duration);
	//        qDebug() << "MediaType" << meta_data.value(QMediaMetaData::MediaType);
	//        qDebug() << "FileFormat" << meta_data.value(QMediaMetaData::FileFormat);
	//        qDebug() << "AudioBitRate" << meta_data.value(QMediaMetaData::AudioBitRate);
	//        qDebug() << "AudioCodec" << meta_data.value(QMediaMetaData::AudioCodec);
	//        qDebug() << "TrackNumber" << meta_data.value(QMediaMetaData::TrackNumber);
	//        qDebug() << "Composer" << meta_data.value(QMediaMetaData::Composer);
	//        qDebug() << "LeadPerformer" << meta_data.value(QMediaMetaData::LeadPerformer);
	//        qDebug() << "CoverArtImage" << meta_data.value(QMediaMetaData::CoverArtImage);
	//        qDebug() << "Orientation" << meta_data.value(QMediaMetaData::Orientation);
	//        qDebug() << "Resolution" << meta_data.value(QMediaMetaData::Resolution);

	emit metaDataChanged(metaData);
}
