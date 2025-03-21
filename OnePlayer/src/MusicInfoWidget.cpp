#include "MusicInfoWidget.h"
#include "DMenu.h"
#include "ImageHandler.h"
#include <opencv2/opencv.hpp>
#include "OnePlayerStruct.h"
#include "OneMessageBox.h"
#include <QDesktopServices>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QPropertyAnimation>
#include "settinghandler.h"


const static QStringList IMAGE_MENU_STR_LIST = { "查看封面", "保存封面" };

MusicInfoWidget::MusicInfoWidget(QWidget* parent)
	: QWidget(parent)
	, imageMenu_(new DMenu(IMAGE_MENU_STR_LIST))
{
	ui.setupUi(this);
	ui.label_image->installEventFilter(this); // 安装事件过滤器
    opacityEffect_ = new QGraphicsOpacityEffect(this);
    animation_ = new QPropertyAnimation(opacityEffect_, "opacity");
    animation_->setDuration(MUSIC_INFO_WIDGET_ANIMATION_TIME);
    setGraphicsEffect(opacityEffect_);

	connect(imageMenu_, &DMenu::sigBtnClicked, this, [this](const QString& text)
	{
		if ("查看封面" == text)
			showImage();
		else if ("保存封面" == text)
			saveImage();
	});
	connect(imageMenu_, &DMenu::maybeNeedHide, this, [this]()
	{
		if (!ui.label_image->underMouse())
			imageMenu_->animateHide();
	});
}

MusicInfoWidget::~MusicInfoWidget()
{
	if (imageMenu_)
		imageMenu_->deleteLater();
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

	cv::Mat blurMat = ImageHandler::blurImage(originImage, 40);
	cv::Mat lightMat = ImageHandler::lightImage(blurMat, 0.85);
	QImage lightImage = ImageHandler::cvMatToQImage(lightMat);
	QImage roundImage = ImageHandler::roundImage(lightImage, 20);

    ui.label_background->setPixmap(QPixmap::fromImage(roundImage));

    QString text_color = ImageHandler::getTextColor(originImage);
    //setStyleSheet(QString("QLabel{ color: rgba(%1, 0.8); }").arg(text_color));

	QRgb rgb = ImageHandler::getMainColor(originImage, QRect(50, ui.lyric_widget->y(), ui.lyric_widget->width() - 100, ui.lyric_widget->height()));
	int gray = (qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3;
    ui.lyric_widget->setTextColor(gray > 127);
}

void MusicInfoWidget::drawMainMat(const cv::Mat& image)
{
	mainMat_ = image;
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

bool MusicInfoWidget::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == ui.label_image)
	{
		switch (event->type())
		{
		case QEvent::MouseButtonPress:  // 鼠标按下
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() == Qt::RightButton)
			{
				imageMenu_->show(0);
				return true;
			}
			else if (mouseEvent->button() == Qt::LeftButton)
			{
				imageMenu_->animateHide();
			}
			break;
		}
		case QEvent::MouseButtonDblClick:  // 鼠标双击
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() == Qt::LeftButton)
			{
				showImage();
			}
			return true;
		}
		default:
			break;
		}
	}

	return QWidget::eventFilter(obj, event);
}

void MusicInfoWidget::showImage()
{
	QString tempDir = QCoreApplication::applicationDirPath();
	QString tempFilePath = tempDir + "/temp_image.png";

	if (!cv::imwrite(tempFilePath.toStdString(), mainMat_))
	{
		qWarning() << "Failed to save pixmap to file:" << tempFilePath;
		return;
	}
	if (!QDesktopServices::openUrl(QUrl::fromLocalFile(tempFilePath)))
		qWarning() << "Failed to open file with default viewer.";
}

void MusicInfoWidget::saveImage()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("保存封面")
		, "C:/" + ui.lyric_widget->getLabel1Text() + ".png");
	if (fileName.isEmpty())
	{
		OneMessageBox::warning(this, tr("警告"), tr("选择导出文件路径为空！"));
		return;
	}
}
