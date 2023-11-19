#include "MusicInfoWidget.h"
#include <QLabel>
#include "ImageHandler.h"
#include "OnePlayerStruct.h"
#include <QElapsedTimer>
#include <QPropertyAnimation>



MusicInfoWidget::MusicInfoWidget(QWidget *parent)
	: QWidget(parent)
    , mask_(new QLabel(this))
{
	ui.setupUi(this);
    QGraphicsEffect* op;
}

MusicInfoWidget::~MusicInfoWidget()
{
	
}

void MusicInfoWidget::drawImage(QImage image)
{
    mask_->raise();
    mask_->move(0, 0);
    mask_->setFixedSize(ui.stacked_info->size());
    mask_->show();
    mask_->setStyleSheet("background-color: red;");
    animationHide();
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
    animationShow();
}

void MusicInfoWidget::animationHide()
{
    QPropertyAnimation* animation = new QPropertyAnimation(mask_, "windowOpacity");
    animation->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME*5);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    QEventLoop loop;
    connect(animation, &QPropertyAnimation::finished, this, [this, animation, &loop]()
    {
        //isAnimateHide_ = true;
        animation->deleteLater();
        loop.quit();
    });

    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
    loop.exec();
}

void MusicInfoWidget::animationShow()
{
    QPropertyAnimation* animation = new QPropertyAnimation(mask_, "windowOpacity");
    animation->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME*5);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(animation, &QPropertyAnimation::finished, this, [this, animation]()
    {
        //isAnimateHide_ = true;
        animation->deleteLater();
    });

    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->start();
}
