#pragma once

#include "PlayerBase.h"
#include <QMediaPlayer>

class QAudioOutput;
class QBuffer;
class QMediaPlayer;

class PlayerQt : public PlayerBase
{
	Q_OBJECT

public:
	PlayerQt(QObject* parent = nullptr);
	~PlayerQt();

	virtual bool playOrPause();
	virtual void playNext();
	virtual void playPrevious();
	virtual void setVolume(float vol);
	virtual void setPosition(qint64 pos);
	virtual qint64 getPosition() { return player_->position(); }
	virtual qint64 getDuration() { return player_->duration(); }
	virtual void playCurrentIndex(qint64 pos=0);
	virtual void setLoop(bool loop);

private:
	QMediaPlayer* player_;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QAudioOutput* audioOutput_;
#endif
	// 放歌的开始pos，在playCurrentIndex中修改，在durationChange中设置到player中
	qint64 startPos_;
	QBuffer* dataBuffer_;
};
