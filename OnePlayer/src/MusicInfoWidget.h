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
	void animationHide();

private:
	void animationShow();

	Ui::MusicInfoWidgetClass ui;
	QGraphicsOpacityEffect* opacityEffect_;
	QPropertyAnimation* animation_;
};
