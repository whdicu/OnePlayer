#pragma once

#include <QWidget>
#include "ui_SearchEdit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SearchEditClass; };
QT_END_NAMESPACE

class SearchEdit : public QWidget
{
	Q_OBJECT

public:
	SearchEdit(QWidget *parent = nullptr);
	~SearchEdit();
	void setEditFocus() { ui->le_find->setFocus(); }
	void setEditText(const QString& text) { ui->le_find->setText(text); }

private:
	Ui::SearchEditClass *ui;
};
