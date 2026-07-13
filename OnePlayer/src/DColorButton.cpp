#include "DColorButton.h"
#include "DStyle.h"
#include <QHBoxLayout>
#include <QLabel>
#include "settinghandler.h"


DColorButton::DColorButton(QWidget* parent)
	: QWidget(parent)
	, widget_(new QWidget(this))
	, widgetColor_(new QWidget(this))
	, labelText_(new QLabel(this))
	, hasBorder_(false)
	, isClicked_(false)
{
	QString border = hasBorder_ ? "1px solid #5c5c66" : "none";

	widgetColor_->setFixedWidth(30);
	widgetColor_->setMinimumHeight(30);
	widgetColor_->setStyleSheet(QString("background-color: white; border: %1; border-radius: 15px;").arg(border));

	labelText_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	
	widget_->setObjectName("widget");
	widget_->setStyleSheet(QString("#widget { border: %1; border-radius: 20px; }").arg(border));
	QHBoxLayout* layout1 = new QHBoxLayout(widget_);
	layout1->setSpacing(10);
	layout1->setContentsMargins(5, 5, 5, 5);
	layout1->addWidget(widgetColor_);
	layout1->addWidget(labelText_);
	widget_->setLayout(layout1);

	QHBoxLayout* layout2 = new QHBoxLayout(this);
	layout2->setSpacing(0);
	layout2->setContentsMargins(0, 0, 0, 0);
	layout2->addWidget(widget_);
	setLayout(layout2);
	setStyleSheet("#widget { background-color: white; }");
}

DColorButton::DColorButton(const QColor& color, const QString& text, QWidget* parent /*= nullptr*/)
	: DColorButton(parent)
{
	labelText_->setText(text);
	setColor(color);
}

DColorButton::~DColorButton()
{

}

void DColorButton::setHasBorder(bool hasBorder)
{
	hasBorder_ = hasBorder;
	QString border = hasBorder_ ? "1px solid #5c5c66" : "none";
	widget_->setStyleSheet(QString("#widget { border: %1; border-radius: 20px; }").arg(border));
}

void DColorButton::setColor(const QColor& color)
{
	color_ = color;
	QString border = hasBorder_ ? "1px solid #5c5c66" : "none";
	widgetColor_->setStyleSheet(QString("background-color: %1; border: %2; border-radius: 15px;")
		.arg(DStyle::color2Str(color))
		.arg(border));
}

void DColorButton::setText(const QString& text)
{
	labelText_->setText(text);
}

void DColorButton::setFont(const QFont& font, const QColor& textColor)
{
	labelText_->setFont(font);
	labelText_->setStyleSheet(QString("color: %1;").arg(DStyle::color2Str(textColor)));
}

void DColorButton::enterEvent(QEvent* event)
{
	setCursor(Qt::PointingHandCursor);
	setStyleSheet("#widget { background-color: rgba(255, 255, 255, 0.5); }");
	QWidget::enterEvent(event);
}

void DColorButton::leaveEvent(QEvent* event)
{
	setCursor(Qt::ArrowCursor);
	setStyleSheet("#widget { background-color: white; }");
	QWidget::leaveEvent(event);
}

void DColorButton::mousePressEvent(QMouseEvent* event)
{
	isClicked_ = true;
}

void DColorButton::mouseReleaseEvent(QMouseEvent* event)
{
	if (isClicked_)
		emit sigClicked(color_);
	isClicked_ = false;
}
