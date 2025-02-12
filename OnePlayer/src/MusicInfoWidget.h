#pragma once

#include "opencv2/core.hpp"
#include <QWidget>
#include "ui_MusicInfoWidget.h"

class DMenu;
class QGraphicsOpacityEffect;
class QPropertyAnimation;

class MusicInfoWidget : public QWidget
{
	Q_OBJECT

public:
	MusicInfoWidget(QWidget* parent = nullptr);
	~MusicInfoWidget();
	void drawBGMat(const cv::Mat& image);
	void drawMainMat(const cv::Mat& image);
	void setMusicName(const QString& name) { ui.lyric_widget->setLabel1Text(name); }
	void setSingerName(const QString& name) { ui.lyric_widget->setLabel2Text(name); }
	void setAlbumName(const QString& name) { ui.lyric_widget->setLabel3Text(name); }
	void setLyrics(const QString& lyricStr) { ui.lyric_widget->setLyrics(lyricStr); }
	void setLyricPos(dint64 pos) { ui.lyric_widget->setPos(pos); }
	// 隐藏动画为阻塞式播放，使在动画播放完之后才会去做刷新文字、播放显示动画等操作。
	void animationHide();
	void animationShow();

private:
	bool eventFilter(QObject* obj, QEvent* event) override;
	void showImage();
	void saveImage();

	Ui::MusicInfoWidgetClass ui;
	QGraphicsOpacityEffect* opacityEffect_;
	QPropertyAnimation* animation_;
	cv::Mat mainMat_;
	DMenu* imageMenu_;
};
