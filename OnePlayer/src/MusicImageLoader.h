#pragma once

#include <QHash>
#include <QImage>
#include <QMutex>
#include <QObject>
#include <QPointer>
#include <QQueue>
#include <QString>
#include <QThread>
#include <QWaitCondition>

#include "OnePlayerStruct.h"

class LocalMusicButton;
class MusicImageWorker;

// 后台线程要处理的封面任务
struct MusicImageTask
{
	int id;
	QString musicPath;
	int diameter;
};

// 封面加载器：按钮把任务丢进来，工作线程读完后回到主线程刷新按钮
class MusicImageLoader : public QObject
{
	Q_OBJECT

public:
	static MusicImageLoader* getInstance();

	// receiver 销毁后，该次请求的结果会被自动丢弃
	void requestImage(LocalMusicButton* receiver, const QString& musicPath
		, int diameter=MUSIC_BTN_CIRCLE_SIZE);
	void stop();

private slots:
	void onImageLoaded(int requestId, const QImage& image);

private:
	MusicImageLoader();
	~MusicImageLoader();

	MusicImageWorker* worker_;
	QThread* thread_;
	QHash<int, QPointer<QObject>> receivers_;
	int lastId_;
};

// 工作线程的执行体：任务队列为空时休眠等待
class MusicImageWorker : public QObject
{
	Q_OBJECT

public:
	explicit MusicImageWorker(QObject* parent = nullptr);

	void push(const MusicImageTask& task);  // 线程安全，可在任意线程调用
	void stop();  // 线程安全

public slots:
	void process();

signals:
	void sigLoaded(int requestId, const QImage& image);

private:
	QMutex mutex_;
	QWaitCondition cond_;
	QQueue<MusicImageTask> queue_;
	bool quit_;
};

#define MUSIC_IMAGE_LOADER MusicImageLoader::getInstance()
