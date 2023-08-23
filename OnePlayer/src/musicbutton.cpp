#include "musicbutton.h"
#include "ui_musicbutton.h"

MusicButton::MusicButton(const QUrl& url, QWidget *parent)
    : BaseMusicButton(parent)
    , ui(new Ui::MusicButton)
    , url_(url)
{
    ui->setupUi(this);
    ui->btn_name->setText(url.fileName());
}

MusicButton::~MusicButton()
{
    delete ui;
}
