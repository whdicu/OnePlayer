#include "PlayListButton.h"
#include "ui_PlayListButton.h"

PlayListButton::PlayListButton(const QString& str, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayListButton)
{
    ui->setupUi(this);
    ui->btn_name->setText(str);
}

PlayListButton::~PlayListButton()
{
    delete ui;
}

void PlayListButton::on_btn_name_clikcked()
{
	QString str = ui->btn_name->text();
	emit sigPlayListClicked(str);
}

void PlayListButton::on_btn_delete_clikcked()
{
	QString str = ui->btn_delete->text();
	emit sigDeleteClicked(str);
}
