#include "Toast.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

const static int ANIMATION_TIME = 200;


void Toast::makeToast(const QString& str, int time)
{
	int x = QCursor::pos().x();
	int y = QCursor::pos().y();
	Toast* t = new Toast(str, time);
	t->move(x, y);
	t->animationShow();
}

Toast::Toast(const QString& str, int time)
	: QWidget(nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
	, isHide_(false)
{
	setAttribute(Qt::WA_TranslucentBackground);

	animation_ = new QPropertyAnimation(this, "windowOpacity");
	animation_->setDuration(ANIMATION_TIME);
	connect(animation_, &QPropertyAnimation::finished, this, [this]()
	{
		if (isHide_)
			deleteLater();
	});

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(QMargins(0, 0, 0, 0));

	QLabel* label = new QLabel(str, this);
	label->setFixedHeight(36);
	label->setStyleSheet("QLabel { \
		padding-left: 10px; \
		padding-right: 10px; \
		color: #5c5c66; \
		background-color: white; \
		border-radius: 18px; }");

	layout->addWidget(label);

	QTimer::singleShot(time, this, &Toast::animationHide);
}

Toast::~Toast()
{
	
}

void Toast::animationShow()
{
	isHide_ = false;
	animation_->setStartValue(0.0);
	animation_->setEndValue(1.0);
	animation_->start();
	show();
}

void Toast::animationHide()
{
	isHide_ = true;
	animation_->setStartValue(1.0);
	animation_->setEndValue(0.0);
	animation_->start();
}
