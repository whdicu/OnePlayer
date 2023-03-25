#ifndef BASEMUSICBUTTON_H
#define BASEMUSICBUTTON_H

#include <QWidget>

class BaseMusicButton : public QWidget
{
    Q_OBJECT

public:
    explicit BaseMusicButton(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual const QUrl& get_url() const = 0;
    virtual QString get_filename() const = 0;
    virtual void setNormalStyle() = 0;
    virtual void setPlayingStyle() = 0;

signals:
    void clicked();
};

#endif // BASEMUSICBUTTON_H
