#include "NeteaseMusicBtn.h"
#include "ui_NeteaseMusicBtn.h"

NeteaseMusicBtn::NeteaseMusicBtn(const NeteaseSongInfo& info, QWidget *parent)
    : BaseMusicButton(parent)
    , ui(new Ui::NeteaseMusicBtn)
    , is_playing_(false)
	, info_(info)
{
    ui->setupUi(this);
    ui->btn_name->setText(info.name);
    ui->btn_singer->setText(info.singer);
}

NeteaseMusicBtn::~NeteaseMusicBtn()
{
    delete ui;
}

void NeteaseMusicBtn::enterEvent(QEnterEvent *)
{
    if (! is_playing_)
        setStyleSheet(NETEASE_HOVER_STYLE);
}

void NeteaseMusicBtn::leaveEvent(QEvent *)
{
    if (! is_playing_)
        setStyleSheet(NETEASE_NORMAL_STYLE);
}
