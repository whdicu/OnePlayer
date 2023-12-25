#include "MusicInfoWidget.h"
#include <QLabel>
#include "ImageHandler.h"
#include "OnePlayerStruct.h"
#include <QElapsedTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>


MusicInfoWidget::MusicInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    opacityEffect_ = new QGraphicsOpacityEffect(this);
    animation_ = new QPropertyAnimation(opacityEffect_, "opacity");
    animation_->setDuration(MUSIC_INFO_WIDGET_ANIMATION_TIME);
    setGraphicsEffect(opacityEffect_);
}

MusicInfoWidget::~MusicInfoWidget()
{
	
}

void MusicInfoWidget::drawImage(QImage image)
{
	if (image.isNull())
	{
		DWarning << "image is null!";
		return;
	}

    ui.label_image->setPixmap(ImageHandler::cutImage(image, ui.label_image->width()
        , ui.label_image->height(), 20, false, 1.0));

	ui.label_background->setPixmap(ImageHandler::cutImage(image, ui.label_background->width()
        , ui.label_background->height(), 20, true, 0.85));

    QString text_color = ImageHandler::getTextColor(image);
    //setStyleSheet(QString("QLabel{color: %1;}").arg(text_color));
    setStyleSheet(QString("QLabel{ color: rgba(%1, 0.8); }").arg(text_color));
    //ui->lyrics_widget->set_color(text_color == "#5c5c66");
}

void MusicInfoWidget::animationHide()
{
	animation_->stop();

    static QEventLoop loop;
    connect(animation_, &QPropertyAnimation::finished, this, []()
    {
        loop.quit();
    });
    
	animation_->setEasingCurve(MUSIC_INFO_WIDGET_HIDE_EASING);
    animation_->setStartValue(opacityEffect_->opacity());
    animation_->setEndValue(0.0);
    animation_->start();
    loop.exec();
}

void MusicInfoWidget::animationShow()
{
	animation_->stop();

	animation_->setEasingCurve(MUSIC_INFO_WIDGET_SHOW_EASING);
    animation_->setStartValue(opacityEffect_->opacity());
    animation_->setEndValue(1.0);
    animation_->start();
}
