#include "PlayListEdit.h"
#include <QKeyEvent>


PlayListEdit::PlayListEdit(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

PlayListEdit::~PlayListEdit()
{
}

void PlayListEdit::on_btn_add_clicked()
{
	QString playListName = ui.le->text();
	emit sigAdd(playListName);
}
