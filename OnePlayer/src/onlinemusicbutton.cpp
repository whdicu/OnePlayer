#include "onlinemusicbutton.h"
#include "ui_onlinemusicbutton.h"

OnlineMusicButton::OnlineMusicButton(const MusicInfo& music, QWidget *parent)
    : BaseMusicButton(parent)
    , ui(new Ui::OnlineMusicButton)
    , music_info_(music)
    , is_playing_(false)
{
    ui->setupUi(this);
    ui->btn_name->setText(music.name_);
    ui->btn_singer->setText(music.singer_);
}

OnlineMusicButton::~OnlineMusicButton()
{
    delete ui;
}

void OnlineMusicButton::enterEvent(QEnterEvent *)
{
    if (! is_playing_)
        setStyleSheet(ONLINE_HOVER_STYLE);
}

void OnlineMusicButton::leaveEvent(QEvent *)
{
    if (! is_playing_)
        setStyleSheet(ONLINE_NORMAL_STYLE);
}
