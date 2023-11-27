#include "OneMessageBox.h"
#include <QPushButton>

int OneMessageBox::information(QWidget* parent, const QString& title, const QString& text, OneMessageBoxButton btn)
{
	OneMessageBox box(parent, title, text, INFORMATION_BOX, btn);
	return box.exec();
}

int OneMessageBox::warning(QWidget* parent, const QString& title, const QString& text, OneMessageBoxButton btn)
{
	OneMessageBox box(parent, title, text, WARNING_BOX, btn);
	return box.exec();
}

int OneMessageBox::critical(QWidget* parent, const QString& title, const QString& text, OneMessageBoxButton btn)
{
	OneMessageBox box(parent, title, text, CRITICAL_BOX, btn);
	return box.exec();
}

OneMessageBox::OneMessageBox(QWidget* parent, const QString& title, const QString& text, OneMessageBoxType type, OneMessageBoxButton btn)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowTitle(title);

	ui.label2->setText(text);

	connect(ui.btn_accept, &QPushButton::clicked, this, &QDialog::accept);
	connect(ui.btn_cancel, &QPushButton::clicked, this, &QDialog::reject);

	if (btn & ACCEPT_BTN)
		ui.btn_accept->show();
	else
		ui.btn_accept->hide();

	if (btn & CANCEL_BTN)
		ui.btn_cancel->show();
	else
		ui.btn_cancel->hide();

	//QString image_url = "";
	//switch (type)
	//{
	//case INFORMATION_BOX:
	//	image_url = ":/NBIcon/information.png";
	//	break;
	//case WARNING_BOX:
	//	image_url = ":/NBIcon/warning.png";
	//	break;
	//case CRITICAL_BOX:
	//	image_url = ":/NBIcon/critical.png";
	//	break;
	//}

	//QPixmap pixmap = QPixmap::fromImage(QImage(image_url).scaled(ui.label_logo->size()));
	//ui.label_logo->setPixmap(pixmap);
}

OneMessageBox::~OneMessageBox()
{}
