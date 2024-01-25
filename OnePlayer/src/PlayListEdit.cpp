#include "PlayListEdit.h"
#include <QKeyEvent>


PlayListEdit::PlayListEdit(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.le, &QLineEdit::returnPressed, this, &PlayListEdit::on_btn_add_clicked);
}

PlayListEdit::~PlayListEdit()
{
}

void PlayListEdit::on_btn_add_clicked()
{
	QString playListName = ui.le->text();
	if (playListName.isEmpty())
		return;
	emit sigAdd(playListName);
}
