#include "PlayListButton.h"
#include "ui_PlayListButton.h"
#include "settinghandler.h"

PlayListButton* PlayListButton::choosedBtn_ = nullptr;

PlayListButton::PlayListButton(const NeteasePlayListInfo& info, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayListButton)
	, info_(info)
{
    ui->setupUi(this);
    ui->btn_name->setText(info.name);
}

PlayListButton::~PlayListButton()
{
	if (this == choosedBtn_)
		choosedBtn_ = nullptr;
    delete ui;
}

void PlayListButton::setChoosed()
{
	PlayListButton* oldBtn = choosedBtn_;
	choosedBtn_ = this;

	// restore the old button to normal style
	if (oldBtn && oldBtn != this)
		oldBtn->setColor(oldBtn->oldColor_);

	// show the playing style on this button
	setColor(oldColor_);
}

void PlayListButton::setColor(const QColor& color)
{
	if (!color.isValid())
	{
		qWarning() << __FUNCTION__ << "color invalid!";
		return;
	}

	oldColor_ = color;

	// keep the playing style when this button is the current one
	QString style = (this == choosedBtn_) ? PLAY_LIST_PLAYING_STYLE : PLAY_LIST_NORMAL_STYLE;
	setStyleSheet(style.arg(oldColor_.red()).arg(oldColor_.green()).arg(oldColor_.blue())
		+ PLAY_LIST_BTNS_STYLE);
}

void PlayListButton::on_btn_name_clicked()
{
	setChoosed();
	QString str = ui->btn_name->text();
	if (-1 == info_.id)
		emit sigPlayListClicked(str);
	else
		emit sigNeteasePlayListClicked(info_);
}

void PlayListButton::on_btn_delete_clicked()
{
	QString str = ui->btn_name->text();
	emit sigDeleteClicked(str);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void PlayListButton::enterEvent(QEnterEvent*)
#else
void PlayListButton::enterEvent(QEvent*)
#endif
{
	if (this == choosedBtn_)
		return;

	setStyleSheet(PLAY_LIST_HOVER_STYLE.arg(oldColor_.red()).arg(oldColor_.green()).arg(oldColor_.blue())
		+ PLAY_LIST_BTNS_STYLE);
}

void PlayListButton::leaveEvent(QEvent*)
{
	if (this == choosedBtn_)
		return;

	setStyleSheet(PLAY_LIST_NORMAL_STYLE.arg(oldColor_.red()).arg(oldColor_.green()).arg(oldColor_.blue())
		+ PLAY_LIST_BTNS_STYLE);
}
