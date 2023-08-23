#include "morewidget.h"
#include "ui_morewidget.h"

MoreWidget::MoreWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoreWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

}

MoreWidget::~MoreWidget()
{
    delete ui;
}
