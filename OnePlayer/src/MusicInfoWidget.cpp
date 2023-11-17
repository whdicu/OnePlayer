#include "MusicInfoWidget.h"
#include <QLabel>
#include "ImageHandler.h"
#include <QElapsedTimer>



MusicInfoWidget::MusicInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

MusicInfoWidget::~MusicInfoWidget()
{
	
}

void MusicInfoWidget::drawImage(QImage image)
{
    static QImage default_image(":/images/music.png");
    if (image.isNull())
        image = default_image;
    
    ui.label_image->setPixmap(ImageHandler::cutImage(image, ui.label_image->width()
        , ui.label_image->height(), 20, false, 1.0));

	ui.label_background->setPixmap(ImageHandler::cutImage(image, ui.label_background->width()
        , ui.label_background->height(), 20, true, 0.85));

    QString text_color = ImageHandler::getTextColor(image);
    setStyleSheet(QString("QLabel{color: %1;}").arg(text_color));
    //ui->lyrics_widget->set_color(text_color == "#5c5c66");
}
