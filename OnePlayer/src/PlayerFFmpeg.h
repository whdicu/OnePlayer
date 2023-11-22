#pragma once

#include "OnePlayerStruct.h"
#include "PlayerBase.h"

class PlayerFFmpeg : public PlayerBase
{
	Q_OBJECT

public:
	static MusicInfo getMusicInfo(const QString& musicPath);
	PlayerFFmpeg(QObject* parent = nullptr);
	~PlayerFFmpeg();

	virtual bool playOrPause();
	virtual void stop();
	virtual void playNext();
	virtual void playPrevious();
	virtual void setVolume(float vol);
	virtual void setPosition(qint64 pos);
	virtual qint64 getPosition() { return 0; }
	virtual qint64 getDuration() { return 0; }
	virtual void playCurrentIndex() {}
	virtual void setLoop(bool loop) {};

private:

};
