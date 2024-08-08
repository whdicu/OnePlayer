#include "DSystemTrayMenu.h"
#include <QEnterEvent>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidgetAction>

const static int MENU_WIDTH = 120;
const static int ITEM_HEIGHT = 32;
const static QSize BTN_ICON_SIZE = QSize(20, 20);
const static QFont FONT = QFont("Microsoft YaHei UI", 10);
const static QString MENU_STYLE_SHEET = QString(R"(
QMenu {
	width: %1px;
	border-radius: 5px;
}
QPushButton {
	color: #5c5c66;
	text-align: left;
	padding-left: 4px;
	background-color: transparent;
}
)").arg(MENU_WIDTH);

const static QString BTN_STYLE_SHEET = R"(
QPushButton {
	background-color: transparent;
})";

DSystemTrayMenu::DSystemTrayMenu(QWidget *parent)
	: QMenu(parent)
	, nameBtn_(nullptr)
	, playBtn_(nullptr)
{
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet(MENU_STYLE_SHEET);

	initNameItem();
	initPlayItem();
	initQuitItem();
}

DSystemTrayMenu::~DSystemTrayMenu()
{
}

void DSystemTrayMenu::setPlaying(bool isPlaying)
{
	if (nullptr != playBtn_)
		playBtn_->setIcon(QIcon(isPlaying ? ":/svgs/pause.svg" : ":svgs/play.svg"));
}

void DSystemTrayMenu::setMusicName(const QString& name)
{
	if (nullptr != nameBtn_)
	{
		nameBtn_->setText(name);
		nameBtn_->setToolTip(name);
	}
}

void DSystemTrayMenu::initNameItem()
{
	auto cursor = Qt::PointingHandCursor;
	QWidgetAction* widgetAction = new QWidgetAction(this);
	QWidget* container = new QWidget(this);
	container->setFixedWidth(MENU_WIDTH);
	QHBoxLayout* layout = new QHBoxLayout(container);
	layout->setMargin(0);
	nameBtn_ = new QPushButton(QIcon(":/svgs/music.svg"), "", container);
	connect(nameBtn_, &QPushButton::clicked, this, &DSystemTrayMenu::sigName);
	nameBtn_->setFont(FONT);
	nameBtn_->setFlat(true);
	nameBtn_->setCursor(cursor);
	nameBtn_->setFixedHeight(ITEM_HEIGHT);
	nameBtn_->setIconSize(BTN_ICON_SIZE);
	layout->addWidget(nameBtn_);
	container->setLayout(layout);
	widgetAction->setDefaultWidget(container);
	addAction(widgetAction);
}

void DSystemTrayMenu::initPlayItem()
{
	QSize btnSize(ITEM_HEIGHT, ITEM_HEIGHT);
	QSize iconSize(ITEM_HEIGHT - 8, ITEM_HEIGHT - 8);
	auto cursor = Qt::PointingHandCursor;
	QWidgetAction* widgetAction = new QWidgetAction(this);
	QWidget* container = new QWidget(this);
	container->setFixedWidth(MENU_WIDTH);
	QHBoxLayout* layout = new QHBoxLayout(container);
	layout->setMargin(0);
	QPushButton* btnPrevioud = new QPushButton(QIcon(":/svgs/previous.svg"), "", container);
	connect(btnPrevioud, &QPushButton::clicked, this, &DSystemTrayMenu::sigPrevioud);
	btnPrevioud->setFlat(true);
	btnPrevioud->setStyleSheet(BTN_STYLE_SHEET);
	btnPrevioud->setCursor(cursor);
	btnPrevioud->setFixedSize(btnSize);
	btnPrevioud->setIconSize(iconSize);
	playBtn_ = new QPushButton(QIcon(":/svgs/play.svg"), "", container);
	connect(playBtn_, &QPushButton::clicked, this, &DSystemTrayMenu::sigPlay);
	playBtn_->setFlat(true);
	playBtn_->setStyleSheet(BTN_STYLE_SHEET);
	playBtn_->setCursor(cursor);
	playBtn_->setFixedSize(btnSize);
	playBtn_->setIconSize(iconSize);
	QPushButton* btnNext = new QPushButton(QIcon(":/svgs/next.svg"), "", container);
	connect(btnNext, &QPushButton::clicked, this, &DSystemTrayMenu::sigNext);
	btnNext->setFlat(true);
	btnNext->setStyleSheet(BTN_STYLE_SHEET);
	btnNext->setCursor(cursor);
	btnNext->setFixedSize(btnSize);
	btnNext->setIconSize(iconSize);
	layout->addWidget(btnPrevioud);
	layout->addWidget(playBtn_);
	layout->addWidget(btnNext);
	container->setLayout(layout);
	widgetAction->setDefaultWidget(container);
	addAction(widgetAction);
}

void DSystemTrayMenu::initQuitItem()
{
	auto cursor = Qt::PointingHandCursor;
	QWidgetAction* widgetAction = new QWidgetAction(this);
	QWidget* container = new QWidget(this);
	container->setFixedWidth(MENU_WIDTH);
	QHBoxLayout* layout = new QHBoxLayout(container);
	layout->setMargin(0);
	QPushButton* btn = new QPushButton(QIcon(":/svgs/shutdown.svg"), tr("退出"), container);
	connect(btn, &QPushButton::clicked, this, &DSystemTrayMenu::sigQuit);
	btn->setFont(FONT);
	btn->setFlat(true);
	btn->setCursor(cursor);
	btn->setFixedHeight(ITEM_HEIGHT);
	btn->setIconSize(BTN_ICON_SIZE);
	layout->addWidget(btn);
	container->setLayout(layout);
	widgetAction->setDefaultWidget(container);
	addAction(widgetAction);
}
