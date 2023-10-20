#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_settingtabwidget.h"
#include "HDBase/DList.hpp"
#include "settinghandler.h"


enum TAB_BUTTON_TYPE
{
	BASE_BTN,
	THEME_BTN
};

const static QMap<TAB_BUTTON_TYPE, QString> csmap_tabButtonTypeStr =
{
	{BASE_BTN, "基础设置"},
	{THEME_BTN, "主题设置"}
};

const static QMap<PLAYER_MODE, QString> csmap_playerModeStr =
{
	{LOCAL, "本地模式"},
	{MYSITE, "网站模式"},
	{ONLINE, "在线模式"},
	{NETEASE, "网易云模式"}
};

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
	void sig_cmb_mode_currentIndexChanged(int index);

private:
	Ui::SettingTabWidget ui;
	DList<QPushButton*> btn_list_;
};
