#ifndef BASEMUSICBUTTON_H
#define BASEMUSICBUTTON_H

#include <QWidget>
#include "HDCore/DType.h"

class BaseMusicButton : public QWidget
{
    Q_OBJECT

public:
    explicit BaseMusicButton(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual void setNormalStyle() = 0;
    virtual void setPlayingStyle() = 0;
    DSizeType getMusicIndex() const { return musicIndex_; }
	void setMusicIndex(DSizeType index) { musicIndex_ = index; }

signals:
    void clicked(DSizeType index);

protected:
    DSizeType musicIndex_;
};

#endif // BASEMUSICBUTTON_H
