#pragma once

#include "PlayerBase.h"
#include <QAudioOutput>
#include <QMediaPlayer>

class PlayerQt : public PlayerBase
{
	Q_OBJECT

public:
	PlayerQt(QObject* parent = nullptr);
	~PlayerQt();

	virtual bool playOrPause();
	virtual void stop();
	virtual void playNext();
	virtual void playPrevious();
	virtual void setVolume(float vol);
	virtual void setPosition(qint64 pos);
	virtual qint64 getPosition() { return player_->position(); }
	virtual qint64 getDuration() { return player_->duration(); }

private slots:
	void slotMetaDataChanged();

private:


	QMediaPlayer* player_;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QAudioOutput* audioOutput_;
#endif
};
