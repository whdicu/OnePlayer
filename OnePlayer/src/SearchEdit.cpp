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
}

SearchEdit::~SearchEdit()
{
	delete ui;
}

void SearchEdit::animationHide()
{
	isAnimateHide_ = true;
	animation_->stop();
	animation_->setStartValue(QRect(x(), y(), width(), height()));
	animation_->setEndValue(QRect(x(), -height(), width(), height()));
	animation_->start();
}

void SearchEdit::animationShow()
{
	isAnimateHide_ = false;
	animation_->stop();
	animation_->setStartValue(QRect(x(), y(), width(), height()));
	animation_->setEndValue(QRect(x(), 0, width(), height()));
	animation_->start();
}
