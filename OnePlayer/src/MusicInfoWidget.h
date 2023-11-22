#pragma once

#include <QWidget>
#include "ui_MusicInfoWidget.h"

class QGraphicsOpacityEffect;
class QPropertyAnimation;

class MusicInfoWidget : public QWidget
{
	Q_OBJECT

public:
	MusicInfoWidget(QWidget *parent = nullptr);
	~MusicInfoWidget();
	void drawImage(QImage image);
	void setMusicName(const QString& name) { ui.label_sound_name->setText(name); }
	void setSingerName(const QString& name) { ui.label_singer->setText(name); }
	void setAlbumName(const QString& name) { ui.label_album->setText(name); }
	// 隐藏动画为阻塞式播放，使在动画播放完之后才会去做刷新文字、播放显示动画等操作。
	void animationHide();
	void animationShow();

private:
	Ui::MusicInfoWidgetClass ui;
	QGraphicsOpacityEffect* opacityEffect_;
	QPropertyAnimation* animation_;
};
