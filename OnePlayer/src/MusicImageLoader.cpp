#include "MusicImageLoader.h"
#include "ImageHandler.h"
#include "LocalMusicButton.h"
#include "NCM/NCMHandler.h"
#include "PlayerFFmpeg.h"
#include <QCoreApplication>

namespace
{
// 把歌曲图片裁剪成圆形
QImage getCircleImage(const QImage& image, int diameter)
{
	if (image.isNull())
		return QImage();

	QImage source = image.convertToFormat(QImage::Format_ARGB32);
	cv::Mat fitMat = ImageHandler::fitImage(ImageHandler::QImageToCvMat(source), diameter, diameter);
	return ImageHandler::roundImage(ImageHandler::cvMatToQImage(fitMat), diameter / 2);
}

// 读取歌曲文件里的图片，ncm是加密文件需要解密后才能拿到图片
QImage getMusicImage(const QString& musicPath)
{
	if (musicPath.endsWith(".ncm"))
		return NCMHandler::getCoverImage(musicPath);

	return PlayerFFmpeg::analyzeMusicInfo(musicPath).image;
}
}


MusicImageLoader* MusicImageLoader::getInstance()
{
	static MusicImageLoader* loader = new MusicImageLoader;
	return loader;
}

MusicImageLoader::MusicImageLoader()
	: worker_(new MusicImageWorker)
	, thread_(new QThread)
	, lastId_(0)
{
	worker_->moveToThread(thread_);
	connect(thread_, &QThread::started, worker_, &MusicImageWorker::process);
	connect(worker_, &MusicImageWorker::sigLoaded, this, &MusicImageLoader::onImageLoaded);
	connect(qApp, &QCoreApplication::aboutToQuit, this, &MusicImageLoader::stop);
	thread_->start();
}

MusicImageLoader::~MusicImageLoader()
{
	stop();
}

void MusicImageLoader::requestImage(LocalMusicButton* receiver, const QString& musicPath
	, int diameter)
{
	if (!receiver || musicPath.isEmpty())
	{
		qWarning() << __FUNCTION__ << "invalid request, music:" << musicPath;
		return;
	}

	MusicImageTask task;
	task.id = ++lastId_;
	task.musicPath = musicPath;
	task.diameter = diameter;

	receivers_.insert(task.id, receiver);
	worker_->push(task);
}

// 工作线程读完图片后回到主线程刷新按钮
void MusicImageLoader::onImageLoaded(int requestId, const QImage& image)
{
	QPointer<QObject> receiver = receivers_.take(requestId);
	if (receiver.isNull())
		return;

	LocalMusicButton* btn = qobject_cast<LocalMusicButton*>(receiver.data());
	if (!btn)
		return;

	btn->setMusicCover(image);
}

void MusicImageLoader::stop()
{
	worker_->stop();
	thread_->quit();
	thread_->wait();
}


MusicImageWorker::MusicImageWorker(QObject* parent)
	: QObject(parent)
	, quit_(false)
{
}

void MusicImageWorker::push(const MusicImageTask& task)
{
	QMutexLocker locker(&mutex_);
	queue_.enqueue(task);
	cond_.wakeOne();
}

void MusicImageWorker::stop()
{
	QMutexLocker locker(&mutex_);
	quit_ = true;
	cond_.wakeAll();
}

void MusicImageWorker::process()
{
	while (true)
	{
		MusicImageTask task;
		{
			QMutexLocker locker(&mutex_);
			while (queue_.isEmpty() && !quit_)
				cond_.wait(&mutex_);

			if (quit_)
				return;

			task = queue_.dequeue();
		}

		// 图片为空也要发回结果，调用方靠它清理记录
		emit sigLoaded(task.id, getCircleImage(getMusicImage(task.musicPath), task.diameter));
	}
}
