#include "neteasewidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>


NeteaseWidget::NeteaseWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    QPixmap pixmap(":/images/music.png");
    QPixmap resultPixmap(ui.label_image->size());
    resultPixmap.fill(Qt::transparent);
    QPainter painter(&resultPixmap);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    QPainterPath path;  // 绘制路径
    //绘制圆角矩形，其中最后两个参数值的范围为（0-99），就是圆角的px值
    path.addRoundedRect(0, 0, ui.label_image->width(), ui.label_image->height(), 10, 10);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, ui.label_image->width(), ui.label_image->height()
        , pixmap.scaled(resultPixmap.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    ui.label_image->setPixmap(resultPixmap);
}

NeteaseWidget::~NeteaseWidget()
{
}
