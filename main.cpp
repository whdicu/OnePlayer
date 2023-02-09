#include "widget.h"

#include <QApplication>
#include <QMessageBox>

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
    w.show();
    return a.exec();
}
