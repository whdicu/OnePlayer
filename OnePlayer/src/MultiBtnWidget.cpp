#include "MultiBtnWidget.h"
#include "ImageHandler.h"
#include "OnePlayerStruct.h"
#include <QPropertyAnimation>
#include <QTimer>


MultiBtnWidget::MultiBtnWidget(QWidget *parent)
	: QWidget(parent)
    , isAnimateHide_(false)
{
	ui.setupUi(this);

	// 内部按钮的动画
	endPos1_ = ui.btn_netease->pos();
	animation1_ = new QPropertyAnimation(ui.btn_netease, "pos");
	animation1_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * (44 - endPos1_.y() / 10) / 44);

	endPos2_ = ui.btn_add->pos();
	animation2_ = new QPropertyAnimation(ui.btn_add, "pos");
	animation2_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * (44 - endPos2_.y() / 10) / 44);

	endPos3_ = ui.btn_play_list->pos();
	animation3_ = new QPropertyAnimation(ui.btn_play_list, "pos");
	animation3_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * (44 - endPos3_.y() / 10) / 44);

	endPos4_ = ui.btn_search->pos();
	animation4_ = new QPropertyAnimation(ui.btn_search, "pos");
	animation4_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * (44 - endPos4_.y() / 10) / 44);

	endPos5_ = ui.btn_setting_4->pos();
	animation5_ = new QPropertyAnimation(ui.btn_setting_4, "pos");
	animation5_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * (44 - endPos5_.y() / 10) / 44);

	endPos6_ = ui.btn_setting_5->pos();
	animation6_ = new QPropertyAnimation(ui.btn_setting_5, "pos");
	animation6_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * (44 - endPos6_.y() / 10) / 44);

	endPos7_ = ui.btn_setting->pos();
	animation7_ = new QPropertyAnimation(ui.btn_setting, "pos");
	animation7_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME * (44 - endPos7_.y() / 10) / 44);

	connect(ui.btn_netease, &QPushButton::clicked, this, &MultiBtnWidget::sigBtnNeteaseClicked);
	connect(ui.btn_play_list, &QPushButton::clicked, this, &MultiBtnWidget::sigBtnPlayListClicked);
	connect(ui.btn_setting, &QPushButton::clicked, this, &MultiBtnWidget::sigBtnSettingClicked);
	connect(ui.btn_search, &QPushButton::clicked, this, &MultiBtnWidget::sigBtnSearchClicked);

	// 未实现
	//ui.btn_netease->hide();
	ui.btn_add->hide();
	ui.btn_setting_4->hide();
	ui.btn_setting_5->hide();
	//ui.btn_search->hide();
}

MultiBtnWidget::~MultiBtnWidget()
{}

void MultiBtnWidget::animationHide()
{
	isAnimateHide_ = true;
	animation1_->stop();
	animation2_->stop();
	animation3_->stop();
	animation4_->stop();
	animation5_->stop();
	animation6_->stop();
	animation7_->stop();

	QPoint endPoint(0, MORE_BTN_WIDGET_HEIGHT + 40);
	animation1_->setStartValue(QPoint(ui.btn_netease->x(), ui.btn_netease->y()));
	animation1_->setEndValue(endPoint);
	animation1_->setEasingCurve(QEasingCurve::InCubic);

	animation2_->setStartValue(QPoint(ui.btn_add->x(), ui.btn_add->y()));
	animation2_->setEndValue(endPoint);
	animation2_->setEasingCurve(QEasingCurve::InCubic);

	animation3_->setStartValue(QPoint(ui.btn_play_list->x(), ui.btn_play_list->y()));
	animation3_->setEndValue(endPoint);
	animation3_->setEasingCurve(QEasingCurve::InCubic);

	animation4_->setStartValue(QPoint(ui.btn_search->x(), ui.btn_search->y()));
	animation4_->setEndValue(endPoint);
	animation4_->setEasingCurve(QEasingCurve::InCubic);

	animation5_->setStartValue(QPoint(ui.btn_setting_4->x(), ui.btn_setting_4->y()));
	animation5_->setEndValue(endPoint);
	animation5_->setEasingCurve(QEasingCurve::InCubic);

	animation6_->setStartValue(QPoint(ui.btn_setting_5->x(), ui.btn_setting_5->y()));
	animation6_->setEndValue(endPoint);
	animation6_->setEasingCurve(QEasingCurve::InCubic);

	animation7_->setStartValue(QPoint(ui.btn_setting->x(), ui.btn_setting->y()));
	animation7_->setEndValue(endPoint);
	animation7_->setEasingCurve(QEasingCurve::InCubic);

	QTimer::singleShot(MORE_BTN_WIDGET_ANIMATION_TIME * endPos1_.y() / 10 / 44, [this]()
	{
		if (isAnimateHide_)
			animation1_->start();
	});
	QTimer::singleShot(MORE_BTN_WIDGET_ANIMATION_TIME * endPos2_.y() / 10 / 44, [this]()
	{
		if (isAnimateHide_)
			animation2_->start();
	});
	QTimer::singleShot(MORE_BTN_WIDGET_ANIMATION_TIME * endPos3_.y() / 10 / 44, [this]()
	{
		if (isAnimateHide_)
			animation3_->start();
	});
	QTimer::singleShot(MORE_BTN_WIDGET_ANIMATION_TIME * endPos4_.y() / 10 / 44, [this]()
	{
		if (isAnimateHide_)
			animation4_->start();
	});
	QTimer::singleShot(MORE_BTN_WIDGET_ANIMATION_TIME * endPos5_.y() / 10 / 44, [this]()
	{
		if (isAnimateHide_)
			animation5_->start();
	});
	QTimer::singleShot(MORE_BTN_WIDGET_ANIMATION_TIME * endPos6_.y() / 10 / 44, [this]()
	{
		if (isAnimateHide_)
			animation6_->start();
	});
	QTimer::singleShot(MORE_BTN_WIDGET_ANIMATION_TIME * endPos7_.y() / 10 / 44, [this]()
	{
		if (isAnimateHide_)
			animation7_->start();
	});
	QTimer::singleShot(MORE_BTN_WIDGET_ANIMATION_TIME, [this]()
	{
		if (isAnimateHide_)
			move(MORE_BTN_WIDGET_X, MORE_BTN_WIDGET_Y + MORE_BTN_WIDGET_HEIGHT);
	});
}

void MultiBtnWidget::animationShow()
{
	isAnimateHide_ = false;
	animation1_->stop();
	animation2_->stop();
	animation3_->stop();
	animation4_->stop();
	animation5_->stop();
	animation6_->stop();
	animation7_->stop();

	move(MORE_BTN_WIDGET_X, MORE_BTN_WIDGET_Y);

	animation1_->setStartValue(QPoint(ui.btn_netease->x(), ui.btn_netease->y()));
	animation1_->setEndValue(endPos1_);
	animation1_->setEasingCurve(QEasingCurve::OutCubic);

	animation2_->setStartValue(QPoint(ui.btn_add->x(), ui.btn_add->y()));
	animation2_->setEndValue(endPos2_);
	animation2_->setEasingCurve(QEasingCurve::OutCubic);

	animation3_->setStartValue(QPoint(ui.btn_play_list->x(), ui.btn_play_list->y()));
	animation3_->setEndValue(endPos3_);
	animation3_->setEasingCurve(QEasingCurve::OutCubic);

	animation4_->setStartValue(QPoint(ui.btn_search->x(), ui.btn_search->y()));
	animation4_->setEndValue(endPos4_);
	animation4_->setEasingCurve(QEasingCurve::OutCubic);

	animation5_->setStartValue(QPoint(ui.btn_setting_4->x(), ui.btn_setting_4->y()));
	animation5_->setEndValue(endPos5_);
	animation5_->setEasingCurve(QEasingCurve::OutCubic);

	animation6_->setStartValue(QPoint(ui.btn_setting_5->x(), ui.btn_setting_5->y()));
	animation6_->setEndValue(endPos6_);
	animation6_->setEasingCurve(QEasingCurve::OutCubic);

	animation7_->setStartValue(QPoint(ui.btn_setting->x(), ui.btn_setting->y()));
	animation7_->setEndValue(endPos7_);
	animation7_->setEasingCurve(QEasingCurve::OutCubic);

	animation1_->start();
	animation2_->start();
	animation3_->start();
	animation4_->start();
	animation5_->start();
	animation6_->start();
	animation7_->start();
}

void MultiBtnWidget::moveToHide()
{
	isAnimateHide_ = true;
	move(MORE_BTN_WIDGET_X, MORE_BTN_WIDGET_Y + MORE_BTN_WIDGET_HEIGHT);

	QPoint endPoint(0, MORE_BTN_WIDGET_HEIGHT + 40);
	ui.btn_netease->move(endPoint);
	ui.btn_add->move(endPoint);
	ui.btn_play_list->move(endPoint);
	ui.btn_search->move(endPoint);
	ui.btn_setting_4->move(endPoint);
	ui.btn_setting_5->move(endPoint);
	ui.btn_setting->move(endPoint);
}

void MultiBtnWidget::setBtnNeteaseInfo(const QImage& image, const QString& tips)
{
	if (image.isNull())
		return;
	cv::Mat mat = ImageHandler::QImageToCvMat(image);
	cv::Mat fitMat = ImageHandler::fitImage(mat, ui.btn_netease->width(), ui.btn_netease->height());
	QImage fitImage = ImageHandler::cvMatToQImage(fitMat);
	QImage roundImage = ImageHandler::roundImage(fitImage, ui.btn_netease->height() / 2);

	QIcon icon(QPixmap::fromImage(roundImage));
	ui.btn_netease->setIconSize(ui.btn_netease->size());
	ui.btn_netease->setIcon(icon);
	ui.btn_netease->setToolTip(tips);
}

void MultiBtnWidget::setBtnPlayListIcon(const QIcon & icon)
{
	ui.btn_play_list->setIcon(icon);
}
