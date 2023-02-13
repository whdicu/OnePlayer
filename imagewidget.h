#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QGraphicsBlurEffect>
#include <QPixmap>
#include <QWidget>

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    ImageWidget(QWidget* parent = nullptr);
    ~ImageWidget();

    void setPixmap(const QPixmap& pixmap);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap m_pixmap;
    QGraphicsBlurEffect* m_pBlurEffect = nullptr;  // 背景模糊效果
};

#endif // IMAGEWIDGET_H
