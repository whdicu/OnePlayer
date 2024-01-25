#include "PlayListButton.h"
#include "ui_PlayListButton.h"
#include "settinghandler.h"

PlayListButton* PlayListButton::choosedBtn_ = nullptr;

PlayListButton::PlayListButton(const QString& str, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayListButton)
{
    ui->setupUi(this);
    ui->btn_name->setText(str);
}

PlayListButton::~PlayListButton()
{
	if (this == choosedBtn_)
		choosedBtn_ = nullptr;
    delete ui;
}

void PlayListButton::setChoosed()
{
	if (choosedBtn_)
		choosedBtn_->setStyleSheet(PLAY_LIST_NORMAL_STYLE + PLAY_LIST_BTNS_STYLE);
	setStyleSheet(PLAY_LIST_PLAYING_STYLE + PLAY_LIST_BTNS_STYLE);
	choosedBtn_ = this;
}

void PlayListButton::on_btn_name_clicked()
{
	setChoosed();
	QString str = ui->btn_name->text();
	emit sigPlayListClicked(str);
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
	if (SETTING_HANDLER->getStruct().playListName != ui->btn_name->text())
		setStyleSheet(PLAY_LIST_HOVER_STYLE + PLAY_LIST_BTNS_STYLE);
}

void PlayListButton::leaveEvent(QEvent*)
{
	if (SETTING_HANDLER->getStruct().playListName != ui->btn_name->text())
		setStyleSheet(PLAY_LIST_NORMAL_STYLE + PLAY_LIST_BTNS_STYLE);
}
