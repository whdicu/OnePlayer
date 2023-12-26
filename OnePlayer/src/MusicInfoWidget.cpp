#include "MusicInfoWidget.h"
#include <QLabel>
#include "ImageHandler.h"
#include "OnePlayerStruct.h"
#include <QElapsedTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "settinghandler.h"


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

void MusicInfoWidget::drawBGMat(const cv::Mat& image)
{
	if (image.empty())
	{
		DWarning << "image is empty!";
		return;
	}

	cv::Mat originImage;
	switch (SETTING_HANDLER->getStruct().bgMode)
	{
	case FULL_WIDGET:  // 全屏背景图时，传进来的是背景图一样大的图片
	{
		// 从背景图中提取需要的部分，贴合效果
		QRect r(x(), y(), MUSIC_INFO_WIDGET_WIDTH, MUSIC_INFO_WIDGET_HEIGHT);
		originImage = ImageHandler::cutImage(image, r);
		break;
	}
	default:
		originImage = ImageHandler::fitImage(image, MUSIC_INFO_WIDGET_WIDTH, MUSIC_INFO_WIDGET_HEIGHT);
		break;
	}

	cv::Mat blurMat = ImageHandler::blurImage(originImage, 35);
	cv::Mat lightMat = ImageHandler::lightImage(blurMat, 0.9);
	QImage lightImage = ImageHandler::cvMatToQImage(lightMat);
	QImage roundImage = ImageHandler::roundImage(lightImage, 20);

    ui.label_background->setPixmap(QPixmap::fromImage(roundImage));

    QString text_color = ImageHandler::getTextColor(originImage);
    setStyleSheet(QString("QLabel{ color: rgba(%1, 0.8); }").arg(text_color));
    //ui->lyrics_widget->set_color(text_color == "#5c5c66");
}

void MusicInfoWidget::drawMainMat(const cv::Mat& image)
{
	cv::Mat fitMat = ImageHandler::fitImage(image, ui.label_image->width()
		, ui.label_image->height());
	QImage fitImage = ImageHandler::cvMatToQImage(fitMat);
	QImage retImage = ImageHandler::roundImage(fitImage, 20);

	ui.label_image->setPixmap(QPixmap::fromImage(retImage));
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
