#include "NeteaseMusicBtn.h"
#include "ui_NeteaseMusicBtn.h"

NeteaseMusicBtn::NeteaseMusicBtn(const NeteaseSongInfo& info, QWidget *parent)
    : BaseMusicButton(parent)
    , ui(new Ui::NeteaseMusicBtn)
    , isPlaying_(false)
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

void NeteaseMusicBtn::refreshStyle()
{
	switch (state_)
	{
	case BaseMusicButton::Normal:
		isPlaying_ = false;
		setStyleSheet(NETEASE_NORMAL_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
		break;
	case BaseMusicButton::Playing:
		isPlaying_ = true;
		setStyleSheet(NETEASE_PLAYING_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
		break;
	case BaseMusicButton::Selected:
		break;
	default:
		qWarning() << __FUNCTION__ << "unknow state:" << state_;
		break;
	}
}

void NeteaseMusicBtn::enterEvent(QEnterEvent *)
{
    if (!isPlaying_)
        setStyleSheet(NETEASE_HOVER_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
}

void NeteaseMusicBtn::leaveEvent(QEvent *)
{
    if (!isPlaying_)
        setStyleSheet(NETEASE_NORMAL_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
}
