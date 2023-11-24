#include "SearchEdit.h"
#include "OnePlayerStruct.h"
#include <QPropertyAnimation>
#include <QRegularExpression>
#include "settinghandler.h"

SearchEdit::SearchEdit(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::SearchEditClass())
	, isAnimateHide_(false)
{
	ui->setupUi(this);

	animation_ = new QPropertyAnimation(this, "geometry");
	animation_->setDuration(SEARCH_EDIT_ANIMATION_TIME);
	animation_->setEasingCurve(QEasingCurve::InOutQuad);

	connect(ui->le_find, &QLineEdit::textChanged, this, &SearchEdit::findMusic);
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

void SearchEdit::moveToHide()
{
	isAnimateHide_ = true;
	move(x(), -height());
}

// 查找框内上一个按钮
void SearchEdit::on_btn_left_clicked()
{
	--findIndex_;
	if (findIndex_ >= findIndexList_.size())
		findIndex_ = findIndexList_.size() - 1;

	ui->label_count->setText(QString("%1/%2").arg(findIndex_ + 1).arg(findIndexList_.size()));
	emit focusOnBtnAt(findIndexList_.at(findIndex_));
}

// 查找框内下一个按钮
void SearchEdit::on_btn_right_clicked()
{
	++findIndex_;
	if (findIndex_ >= findIndexList_.size())
		findIndex_ = 0;

	ui->label_count->setText(QString("%1/%2").arg(findIndex_ + 1).arg(findIndexList_.size()));
	emit focusOnBtnAt(findIndexList_.at(findIndex_));
}

void SearchEdit::on_btn_close_clicked()
{
	emit sigBtnCloseClicked();
}

void SearchEdit::findMusic(const QString& word)
{
	ui->label_count->setText("0/0");
	findIndex_ = 0;
	findIndexList_.clear();

	if (word == "")
		return;
	
	DList<QUrl> playList = SETTING_HANDLER->currentPlayList();
	for (DSizeType i = 0; i < playList.size(); ++i)
	{
		QRegularExpression reg(".*" + word + ".*");
		auto ret = reg.match(playList.at(i).fileName());
		if (ret.hasMatch())
		{
			findIndexList_.pushBack(i);
		}
	}

	if (findIndexList_.size() > 0)
	{
		ui->label_count->setText(QString("%1/%2").arg(findIndex_ + 1).arg(findIndexList_.size()));
		emit focusOnBtnAt(findIndexList_.at(findIndex_));
	}
}
