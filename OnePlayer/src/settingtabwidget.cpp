#include "settingtabwidget.h"
#include <QComboBox>
#include <QMap>
#include <QPushButton>


const static QString CHECHED_BTN_STYLE = "border: none; background-color: rgba(182, 209, 200, 0.75); border-top-left-radius: 20px; border-top-right-radius: 20px;";


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

	// 初始化播放器模式下拉框
	for (auto it = csmap_playerModeStr.begin(); it != csmap_playerModeStr.end(); ++it)
	{
		ui.cmb_mode->addItem(it.value(), it.key());
	}
	int index = ui.cmb_mode->findData(SettingHandler::getInstance()->get_player_mode());
	ui.cmb_mode->setCurrentIndex(index);

	//ui.btn_open_dir->setIcon(QIcon(":/svgs/goto.svg"));
	//ui.btn_open_dir_download->setIcon(QIcon(":/svgs/goto.svg"));
	ui.btn_change_dir->setIcon(QIcon(":/svgs/folder.svg"));
	ui.btn_change_dir_download->setIcon(QIcon(":/svgs/folder.svg"));

	connect(ui.btn_open_dir, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_open_dir_clicked);
	connect(ui.btn_change_dir, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_change_dir_clicked);
	connect(ui.btn_open_dir_download, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_open_dir_download_clicked);
	connect(ui.btn_change_dir_download, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_change_dir_download_clicked);
	connect(ui.cmb_mode, &QComboBox::currentIndexChanged, this, &SettingTabWidget::sig_cmb_mode_currentIndexChanged);
}

SettingTabWidget::~SettingTabWidget()
{
}

void SettingTabWidget::setMusicDir(const QString& dir)
{
	ui.btn_open_dir->setText(dir);
}

void SettingTabWidget::setDownloadDir(const QString& dir)
{
	ui.btn_open_dir_download->setText(dir);
}

// 屏蔽鼠标滚动
void QComboBox::wheelEvent(QWheelEvent* e) {}
void QAbstractSpinBox::wheelEvent(QWheelEvent* e) {}
