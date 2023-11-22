#ifndef BASEMUSICBUTTON_H
#define BASEMUSICBUTTON_H

#include <QWidget>

class BaseMusicButton : public QWidget
{
    Q_OBJECT

public:
    explicit BaseMusicButton(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual void setNormalStyle() = 0;
    virtual void setPlayingStyle() = 0;
    qint64 getMusicIndex() const { return musicIndex_; }
	void setMusicIndex(qint64 index) { musicIndex_ = index; }

signals:
    void clicked(qint64 index);

protected:
    qint64 musicIndex_;
};

#endif // BASEMUSICBUTTON_H
