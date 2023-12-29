#pragma once

#include "HDBase/DList.hpp"
#include <QWidget>
#include "ui_SearchEdit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SearchEditClass; };
QT_END_NAMESPACE

class QPropertyAnimation;

class SearchEdit : public QWidget
{
	Q_OBJECT

public:
	SearchEdit(QWidget *parent = nullptr);
	~SearchEdit();
	void setEditFocus() { ui->le_find->setFocus(); }
	void setEditText(const QString& text) { ui->le_find->setText(text); }
	void animationHide();
	void animationShow();
	// 直接move到Hide位置
	void moveToHide();

	bool isAnimateHide() { return isAnimateHide_; }

signals:
	void focusOnBtnAt(DSizeType index);
	void sigBtnCloseClicked();
	void sigKeyEnterClicked(DSizeType index);

private slots:
	void on_btn_left_clicked();
	void on_btn_right_clicked();
	void on_btn_close_clicked();

private:
	void findMusic(const QString& word);
	bool eventFilter(QObject* obj, QEvent* event) override;

	Ui::SearchEditClass *ui;
	QPropertyAnimation* animation_;
	bool isAnimateHide_;

	DSizeType findIndex_;
	DList<DSizeType> findIndexList_;
};
