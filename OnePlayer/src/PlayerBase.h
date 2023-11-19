#pragma once
#include <QImage>
#include <QObject>

struct MusicMetaData
{
	QString title;  // 标题
	QString singers;  // 作家们
	QString albumTitle;  // 专辑标题
};

class PlayerBase : public QObject
{
	Q_OBJECT
public:
	PlayerBase(QObject* parent = nullptr);
	// 调用后，处于正在播放状态返回true
	virtual bool playOrPause() = 0;
	virtual void playNext() = 0;
    virtual void playPrevious() = 0;
	virtual void setVolume(float vol) = 0;
	virtual void setPosition(qint64 pos) = 0;
	virtual qint64 getPosition() = 0;
	virtual qint64 getDuration() = 0;  // 音乐总长度

signals:
	void durationChanged(qint64 position);
	void positionChanged(qint64 position);
	void sourceChanged(const QUrl& media);
	void metaDataChanged(const MusicMetaData& metaData);

	void mediaAtEnd();
	void beginPlay();
};

