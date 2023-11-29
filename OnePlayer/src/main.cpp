#include "widget.h"

#include "DSystemTrayMenu.h"
#include <QApplication>
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
	QObject::connect(&w, &Widget::sigChangeSystemIconToolTip, &trayIcon, &QSystemTrayIcon::setToolTip);
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
	QObject::connect(&a, &QApplication::aboutToQuit, &w, &Widget::uninit);
	QObject::connect(&a, &QApplication::aboutToQuit, &trayIcon, &QSystemTrayIcon::hide);

	// 创建托盘菜单
	DSystemTrayMenu trayMenu;
	

	
	trayIcon.setContextMenu(&trayMenu);
	trayIcon.show();

	w.init();
    w.animateShow();
    return a.exec();
}
