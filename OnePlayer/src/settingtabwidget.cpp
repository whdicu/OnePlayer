#include "settingtabwidget.h"
#include "DColorDialog.h"
#include <QComboBox>
#include <QPushButton>


const static QString CHECHED_BTN_STYLE = "border: none; background-color: rgba(182, 209, 200, 0.75); border-top-left-radius: 20px; border-top-right-radius: 20px;";


SettingTabWidget::SettingTabWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	const SettingStruct& stru = SETTING_HANDLER->getStruct();

	// 初始化TabButton
	int i = 0;
	for (auto it = csmap_tabButtonTypeStr.begin(); it != csmap_tabButtonTypeStr.end(); ++it, ++i)
	{
		QPushButton* btn = new QPushButton(it.value(), this);
		//connect(btn, &QPushButton::clicked, this, [this, i]()
		//{
		//	int old_index = ui.stacked_widget->currentIndex();
		//	btn_list_.at(old_index)->setStyleSheet("");
		//	btn_list_.at(i)->setStyleSheet(CHECHED_BTN_STYLE);
		//	ui.stacked_widget->setCurrentIndex(i);
		//});
		btn->setFont(QFont("Microsoft YaHei UI"));
		btn->setMinimumSize(80, 36);
		btn->setMaximumSize(80, 36);
		ui.btn_widget->layout()->addWidget(btn);
		btn_list_.append(btn);
	}
	static_cast<QHBoxLayout*>(ui.btn_widget->layout())->addStretch();
	ui.stacked_widget->setCurrentIndex(1);
	btn_list_.at(0)->setStyleSheet(CHECHED_BTN_STYLE);

	// 初始化播放器模式下拉框
	for (auto it = csmap_playerModeStr.begin(); it != csmap_playerModeStr.end(); ++it)
	{
		ui.cmb_mode->addItem(it.value(), it.key());
	}
	int index = ui.cmb_mode->findData(stru.playerMode);
	ui.cmb_mode->setCurrentIndex(index);

	// 初始化背景图片显示模式
	for (auto it = csmap_bgModeStr.begin(); it != csmap_bgModeStr.end(); ++it)
	{
		ui.cmb_bg_mode->addItem(it.value(), it.key());
	}
	int index2 = ui.cmb_bg_mode->findData(stru.bgMode);
	ui.cmb_bg_mode->setCurrentIndex(index2);

	ui.cb_play_on_start->setChecked(stru.playOnStart);

	ui.widget_color->setColor(stru.mainColor);
	ui.widget_color->setText(tr("主题颜色"));
	
	//ui.btn_open_dir->setIcon(QIcon(":/svgs/goto.svg"));
	//ui.btn_open_dir_download->setIcon(QIcon(":/svgs/goto.svg"));
	ui.btn_change_dir->setIcon(QIcon(":/svgs/folder.svg"));
	ui.btn_change_dir_download->setIcon(QIcon(":/svgs/folder.svg"));

	connect(ui.widget_color, &DColorButton::sigClicked, this, &SettingTabWidget::slot_widget_color_clicked);
	connect(ui.btn_open_dir, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_open_dir_clicked);
	connect(ui.btn_change_dir, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_change_dir_clicked);
	connect(ui.btn_open_dir_download, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_open_dir_download_clicked);
	connect(ui.btn_change_dir_download, &QPushButton::clicked, this, &SettingTabWidget::sig_btn_change_dir_download_clicked);
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

void SettingTabWidget::on_cmb_mode_currentIndexChanged(int index)
{
	PLAYER_MODE playerMode = (PLAYER_MODE)ui.cmb_mode->itemData(index).toInt();
	emit sigPlayerModeChanged(playerMode);
}

void SettingTabWidget::on_cmb_bg_mode_currentIndexChanged(int index)
{
	// 防止 cmb_workbench 在初始化时写入文件
	if (ui.cmb_bg_mode->count() != csmap_bgModeStr.size())
		return;

	SETTING_HANDLER->getStruct().bgMode = (BG_MODE)ui.cmb_bg_mode->itemData(index).toInt();
	emit sigBGModeChanged(SETTING_HANDLER->getStruct().bgMode);
}

void SettingTabWidget::on_cb_play_on_start_stateChanged(int state)
{
	SETTING_HANDLER->getStruct().playOnStart = (0 != state);
}

void SettingTabWidget::slot_widget_color_clicked(const QColor& color)
{
	SettingStruct& stru = SETTING_HANDLER->getStruct();

	DColorDialog dlg({
		{ QColor(182, 209, 200), tr("绿茵") },
		{ QColor(255, 66, 66), tr("粉墨") },
		{ QColor(190, 207, 216), tr("烟云") },
		{ QColor(252, 252, 243), tr("皂白") },
		{ QColor(50, 60, 55), tr("砚青") },
	});
	QColor newColor;
	if (dlg.selectColor(stru.mainColor, newColor) != QDialog::Accepted)
		return;

	if (newColor != stru.mainColor)
	{
		stru.mainColor = newColor;
		emit sigMainColorChanged(newColor);
	}

	ui.widget_color->setColor(stru.mainColor);
}

// 屏蔽鼠标滚动
void QComboBox::wheelEvent(QWheelEvent* e) {}
void QAbstractSpinBox::wheelEvent(QWheelEvent* e) {}
