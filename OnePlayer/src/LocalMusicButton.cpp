#include "LocalMusicButton.h"
#include "ui_LocalMusicButton.h"

LocalMusicButton::LocalMusicButton(const QString& str, QWidget *parent)
    : BaseMusicButton(parent)
    , ui(new Ui::LocalMusicButton)
{
    ui->setupUi(this);
    ui->btn_name->setText(str);
}

LocalMusicButton::~LocalMusicButton()
{
    delete ui;
}
