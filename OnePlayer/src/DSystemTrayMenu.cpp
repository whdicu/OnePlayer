#include "DSystemTrayMenu.h"

DSystemTrayMenu::DSystemTrayMenu(QWidget *parent)
	: QMenu(parent)
{
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("QMenu { border: 1px solid red; border-radius: 10px; }");

	QAction *quitAction = new QAction(QObject::tr("ÍË³ö"), &a);
	connect(quitAction, &QAction::triggered, &w, &QWidget::close);

	addAction(quitAction);
}

DSystemTrayMenu::~DSystemTrayMenu()
{
}
