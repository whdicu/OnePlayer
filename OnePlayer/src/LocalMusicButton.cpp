#include "LocalMusicButton.h"
#include "ui_LocalMusicButton.h"
#include "MusicImageLoader.h"
#include <QEnterEvent>
#include <QPixmap>

namespace
{
	// 没有读到封面时显示的默认音乐图片，只渲染缩放一次
	QPixmap getDefaultPixmap()
	{
		static const QImage image = QImage(":/svgs/music.svg")
			.scaled(MUSIC_BTN_CIRCLE_SIZE, MUSIC_BTN_CIRCLE_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		return QPixmap::fromImage(image);
	}
}

LocalMusicButton::LocalMusicButton(const QUrl& url, QWidget *parent)
    : BaseMusicButton(parent)
    , ui(new Ui::LocalMusicButton)
{
    ui->setupUi(this);
    ui->btn_name->setText(url.fileName());
    ui->label_circle->setPixmap(getDefaultPixmap());

    // 列表里可能存的是本地路径，也可能是file://链接
    QString musicPath = url.toLocalFile();
    if (musicPath.isEmpty())
        musicPath = url.toString();

    // 封面交给后台线程读取，读到后回调setMusicCover
    MUSIC_IMAGE_LOADER->requestImage(this, musicPath);
}

void LocalMusicButton::setMusicCover(const QImage& image)
{
	if (image.isNull())
		return;

	ui->label_circle->setPixmap(QPixmap::fromImage(image));
}

LocalMusicButton::~LocalMusicButton()
{
    delete ui;
}

void LocalMusicButton::refreshStyle()
{
	switch (state_)
	{
	case BaseMusicButton::Normal:
		setStyleSheet(LOCAL_NORMAL_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
		break;
	case BaseMusicButton::Playing:
		setStyleSheet(LOCAL_PLAYING_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
		break;
	case BaseMusicButton::Selected:
		setStyleSheet(LOCAL_HOVER_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
		break;
	default:
		qWarning() << __FUNCTION__ << "unknow state:" << state_;
		break;
	}
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void LocalMusicButton::enterEvent(QEnterEvent*)
#else
void LocalMusicButton::enterEvent(QEvent*)
#endif
{
	if (state_ == BaseMusicButton::Normal)
		setStyleSheet(LOCAL_HOVER_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
}

void LocalMusicButton::leaveEvent(QEvent *)
{
	if (state_ == BaseMusicButton::Normal)
		setStyleSheet(LOCAL_NORMAL_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
}
