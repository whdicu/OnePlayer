#include "widget.h"

#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QSystemTrayIcon>

void out_argv(int argc, char *argv[])
{
    QString s = "";
    for (int i = 0; i < argc; ++i)
        s += "argv[" + QString::number(i) + "] = " + argv[i] + "\n";

    QMessageBox::information(nullptr, " ", s);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString file_path;
    if (argc == 1)
        file_path = "";
    else
        file_path = QString::fromLocal8Bit(argv[1]);

    Widget w(file_path, nullptr);
	if (!w.initSuccess())
		return -1;

	QSystemTrayIcon trayIcon(QIcon(":icon.ico"));
	trayIcon.setToolTip("OnePlayer");
	QObject::connect(&trayIcon, &QSystemTrayIcon::activated, &w, [&w](QSystemTrayIcon::ActivationReason reason)
	{
		switch (reason)
		{
		case QSystemTrayIcon::Unknown:
			break;
		case QSystemTrayIcon::Context:
			break;
		case QSystemTrayIcon::DoubleClick:
		case QSystemTrayIcon::Trigger:
			if (w.isAnimateHide())
				w.animateShow(false);
			else
				w.animateHide();
			break;
		case QSystemTrayIcon::MiddleClick:
			break;
		default:
			break;
		}
	});

	// 创建托盘菜单
	QMenu *trayMenu = new QMenu();
	QAction *quitAction = new QAction(QObject::tr("退出"), &a);
	QObject::connect(quitAction, &QAction::triggered, &w, &QWidget::close);

	trayMenu->addAction(quitAction);
	trayIcon.setContextMenu(trayMenu);
	trayIcon.show();

    w.animateShow();
    return a.exec();
}
