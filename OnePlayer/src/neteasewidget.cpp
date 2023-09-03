#include "neteasewidget.h"
#include "imagewidget.h"
#include "neteasehandler.h"
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QRegularExpression>


void drawImage(QLabel* canterLabel, ImageWidget* bgMask, QImage img=QImage())
{
    const static QImage default_image(":/images/music.png");

    QPixmap pixmap = QPixmap::fromImage(img.isNull() ? default_image : img);

    QPixmap logoPixmap(canterLabel->size());
    logoPixmap.fill(Qt::transparent);
    QPainter painter(&logoPixmap);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    QPainterPath path;  // 绘制路径
    //绘制圆角矩形，其中最后两个参数值的范围为（0-99），就是圆角的px值
    path.addRoundedRect(0, 0, canterLabel->width(), canterLabel->height(), 10, 10);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, canterLabel->width(), canterLabel->height()
        , pixmap.scaled(logoPixmap.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    canterLabel->setPixmap(logoPixmap);
    bgMask->setPixmap(pixmap);
}


NeteaseWidget::NeteaseWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    // 圆角遮罩
    QWidget* ww = new QWidget(this);
    ww->move(ui.stackedWidget->x()-10, ui.stackedWidget->y() - 10);
    ww->resize(ui.stackedWidget->width() + 20, ui.stackedWidget->height() + 20);
    ww->setStyleSheet("background-color: transparent; border: 10px solid white; border-radius: 30px;");
    ww->stackUnder(ui.stackedWidget);

    drawImage(ui.label_image, ui.image_widget_mask);

    connect(ui.btn_login, &QPushButton::clicked, this, &NeteaseWidget::slotLogin);
    connect(ui.edit_phone_email, &QLineEdit::returnPressed, this, &NeteaseWidget::slotLogin);
    connect(ui.edit_password, &QLineEdit::returnPressed, this, &NeteaseWidget::slotLogin);
}

NeteaseWidget::~NeteaseWidget()
{
}

void NeteaseWidget::on_btn_sign_up_clicked()
{
}

void NeteaseWidget::slotLogin()
{
    QString phoneOrEmail = ui.edit_phone_email->text();
    QString password = ui.edit_password->text();

    if (phoneOrEmail.contains('@'))
    {
        QRegularExpression regex("^\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}\\b$");
        QRegularExpressionMatch match = regex.match(phoneOrEmail);

        if (!match.hasMatch())
        {
            QMessageBox::warning(this, tr("邮箱错误"), tr("请输入正确的邮箱"));
            return;
        }

        if (NeteaseHandler::getInatance()->loginEmail(phoneOrEmail, password))
        {
            // 登陆成功
        }
    }
    else
    {
        unsigned long long i = phoneOrEmail.toULongLong();
        /*if (i < 10000000000ul || i > 19999999999ul)
        {
            QMessageBox::warning(this, tr("手机号错误"), tr("请输入正确的手机号"));
            return;
        }*/
        
        if (NeteaseHandler::getInatance()->loginPhone(phoneOrEmail, password))
        {
            // 登陆成功
        }
    }
}
