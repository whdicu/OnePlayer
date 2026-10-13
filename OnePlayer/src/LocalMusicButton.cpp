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

void LocalMusicButton::refreshStyle()
{
	switch (state_)
	{
	case BaseMusicButton::Normal:
		setStyleSheet(LOCAL_NORMAL_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
		break;
	case BaseMusicButton::Playing:
		setStyleSheet(LOCAL_PLAYING_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
		break;
	case BaseMusicButton::Selected:
		setStyleSheet(LOCAL_SELECT_STYLE.arg(color_.red()).arg(color_.green()).arg(color_.blue()));
		break;
	default:
		qWarning() << __FUNCTION__ << "unknow state:" << state_;
		break;
	}
}
