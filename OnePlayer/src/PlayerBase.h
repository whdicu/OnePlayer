#pragma once
#include "OnePlayerStruct.h"
#include <QImage>
#include <QObject>

class PlayerBase : public QObject
{
	Q_OBJECT
public:
	PlayerBase(QObject* parent = nullptr);
	virtual ~PlayerBase() {}
	// 调用后，处于正在播放状态返回true
	virtual bool playOrPause() = 0;
	virtual void playNext() = 0;
    virtual void playPrevious() = 0;
	virtual void setVolume(float vol) = 0;
	virtual void setPosition(qint64 pos) = 0;
	virtual qint64 getPosition() = 0;
	virtual qint64 getDuration() = 0;  // 音乐总长度
	virtual void playCurrentIndex(qint64 pos=0) = 0;
	virtual void setLoop(bool loop) = 0;
	MusicInfo getMusicInfo() { return musicInfo_; }

signals:
	void durationChanged(qint64 position);
	void positionChanged(qint64 position);
	void sourceChanged(const QUrl& media);

	void mediaAtEnd();
	void beginPlay();

protected:
	MusicInfo musicInfo_;
};

