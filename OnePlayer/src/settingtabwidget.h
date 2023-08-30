#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_settingtabwidget.h"
#include "DList.hpp"


class SettingTabWidget : public QWidget
{
	Q_OBJECT

public:
	SettingTabWidget(QWidget *parent = Q_NULLPTR);
	~SettingTabWidget();
	void setMusicDir(const QString& dir);
	void setDownloadDir(const QString& dir);

signals:
	void sig_btn_open_dir_clicked();
	void sig_btn_change_dir_clicked();
	void sig_btn_open_dir_download_clicked();
	void sig_btn_change_dir_download_clicked();
	void sig_btn_local_clicked();
	void sig_btn_mysite_clicked();
	void sig_btn_online_clicked();

private:
	Ui::SettingTabWidget ui;
	DList<QPushButton*> btn_list_;
};
