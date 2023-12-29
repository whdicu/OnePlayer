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

	ui->le_find->installEventFilter(this);
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
	if (findIndexList_.isEmpty())
		return;

	--findIndex_;
	if (findIndex_ >= findIndexList_.size())
		findIndex_ = findIndexList_.size() - 1;

	ui->label_count->setText(QString("%1/%2").arg(findIndex_ + 1).arg(findIndexList_.size()));
	emit focusOnBtnAt(findIndexList_.at(findIndex_));
}

// 查找框内下一个按钮
void SearchEdit::on_btn_right_clicked()
{
	if (findIndexList_.isEmpty())
		return;

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
		QRegularExpression reg(".*" + word + ".*", QRegularExpression::CaseInsensitiveOption);
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

bool SearchEdit::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if (nullptr == ui->le_find)
			return QObject::eventFilter(obj, event);

		switch (keyEvent->key())
		{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			if (findIndex_ < findIndexList_.size())
				emit sigKeyEnterClicked(findIndexList_.at(findIndex_));
			return true;
		case Qt::Key_Up:
			on_btn_left_clicked();
			return true;
		case Qt::Key_Down:
			on_btn_right_clicked();
			return true;
		default:
			break;
		}
	}

	return QObject::eventFilter(obj, event);
}
