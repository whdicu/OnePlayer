#pragma once

#include <QMenu>

class DSystemTrayMenu : public QMenu
{
	Q_OBJECT

public:
	DSystemTrayMenu(QWidget *parent = nullptr);
	~DSystemTrayMenu();
};
