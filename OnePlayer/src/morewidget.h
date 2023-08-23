#ifndef MOREWIDGET_H
#define MOREWIDGET_H

#include <QWidget>

namespace Ui {
class MoreWidget;
}

class MoreWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MoreWidget(QWidget *parent = nullptr);
    ~MoreWidget();

private:
    Ui::MoreWidget *ui;
};

#endif // MOREWIDGET_H
