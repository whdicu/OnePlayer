#include "settingtabwidget.h"
#include <QMap>
#include <QPushButton>

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

const static QString CHECHED_BTN_STYLE = "border: none; background-color: #b6d1c8; border-top-left-radius: 20px; border-top-right-radius: 20px;";


SettingTabWidget::SettingTabWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// 初始化TabButton
	int i = 0;
	for (auto it = csmap_tabButtonTypeStr.begin(); it != csmap_tabButtonTypeStr.end(); ++it, ++i)
	{
		QPushButton* btn = new QPushButton(it.value(), this);
		connect(btn, &QPushButton::clicked, this, [this, i]()
		{
			int old_index = ui.stacked_widget->currentIndex();
			btn_list_.at(old_index)->setStyleSheet("");
			btn_list_.at(i)->setStyleSheet(CHECHED_BTN_STYLE);
			ui.stacked_widget->setCurrentIndex(i);
		});
		btn->setFont(QFont("Microsoft YaHei UI"));
		btn->setMinimumSize(80, 36);
		btn->setMaximumSize(80, 36);
		ui.btn_widget->layout()->addWidget(btn);
		btn_list_.append(btn);
	}
	static_cast<QHBoxLayout*>(ui.btn_widget->layout())->addStretch();
	ui.stacked_widget->setCurrentIndex(0);
	btn_list_.at(0)->setStyleSheet(CHECHED_BTN_STYLE);

	connect(ui.btn_open_dir, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_open_dir_clicked);
	connect(ui.btn_change_dir, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_change_dir_clicked);
	connect(ui.btn_open_dir_download, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_open_dir_download_clicked);
	connect(ui.btn_change_dir_download, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_change_dir_download_clicked);
	connect(ui.btn_local, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_local_clicked);
	connect(ui.btn_mysite, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_mysite_clicked);
	connect(ui.btn_online, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_online_clicked);
}

SettingTabWidget::~SettingTabWidget()
{
}

void SettingTabWidget::setMusicDir(const QString& dir)
{
	ui.label_dir->setText(dir);
}

void SettingTabWidget::setDownloadDir(const QString& dir)
{
	ui.label_dir_download->setText(dir);
}
