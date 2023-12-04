#include "DSystemTrayMenu.h"

DSystemTrayMenu::DSystemTrayMenu(QWidget *parent)
	: QMenu(parent)
{
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("QMenu { border: 1px solid red; border-radius: 10px; }");

	QAction* quitAction = new QAction(QObject::tr("退出"), this);
	connect(quitAction, &QAction::triggered, this, &DSystemTrayMenu::sigQuit);

	addAction(quitAction);
}

DSystemTrayMenu::~DSystemTrayMenu()
{
}
