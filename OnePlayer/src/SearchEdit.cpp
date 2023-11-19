#include "SearchEdit.h"
#include "OnePlayerStruct.h"
#include <QPropertyAnimation>

SearchEdit::SearchEdit(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::SearchEditClass())
	, isAnimateHide_(false)
{
	ui->setupUi(this);

	animation_ = new QPropertyAnimation(this, "geometry");
	animation_->setDuration(SEARCH_EDIT_ANIMATION_TIME);
	animation_->setEasingCurve(QEasingCurve::InOutQuad);
	connect(animation_, &QPropertyAnimation::finished, this, [this]()
	{
		isAnimateHide_ = !isAnimateHide_;
	});
}

SearchEdit::~SearchEdit()
{
	delete ui;
}

void SearchEdit::animationHide()
{
	animation_->setStartValue(QRect(x(), y(), width(), height()));
	animation_->setEndValue(QRect(x(), -height(), width(), height()));
	animation_->start();
}

void SearchEdit::animationShow()
{
	animation_->setStartValue(QRect(x(), y(), width(), height()));
	animation_->setEndValue(QRect(x(), 0, width(), height()));
	animation_->start();
}
