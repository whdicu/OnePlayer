#include "SearchEdit.h"

SearchEdit::SearchEdit(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::SearchEditClass())
{
	ui->setupUi(this);
}

SearchEdit::~SearchEdit()
{
	delete ui;
}
