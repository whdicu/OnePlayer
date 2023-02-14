#include "imagewidget.h"
#include "ui_imagewidget.h"

#include <QPainter>
#include <QPainterPath>

ImageWidget::ImageWidget(QWidget* parent)
    :QWidget(parent)
{
    // 缩略图背景模糊
    m_pBlurEffect = new QGraphicsBlurEffect;
    m_pBlurEffect->setBlurRadius(30);
    m_pBlurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);
    setGraphicsEffect(m_pBlurEffect);
}

ImageWidget::~ImageWidget()
{

}

void ImageWidget::setPixmap(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
    this->update();
}

void ImageWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter this_painter(this);
    if (!m_pixmap.isNull())
    {
        this_painter.drawPixmap((width() - height()) / 2, 0, height(), height(), m_pixmap);
    }
}
