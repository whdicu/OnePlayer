#ifndef BASEMUSICBUTTON_H
#define BASEMUSICBUTTON_H

#include <QWidget>
#include "HDCore/DType.h"

class BaseMusicButton : public QWidget
{
    Q_OBJECT

public:
	enum State
	{
		Normal,
		Playing,
		Selected
	};
	
	explicit BaseMusicButton(QWidget* parent = nullptr) : QWidget(parent), state_(Normal) {}
    ~BaseMusicButton() {}
	virtual void refreshStyle() = 0;
	void setColor(const QColor& color) { color_ = color; refreshStyle(); }
	virtual void setNormalStyle() { state_ = Normal; refreshStyle(); }
    virtual void setPlayingStyle() { state_ = Playing; refreshStyle(); }
    virtual void setSelectStyle() { state_ = Selected; refreshStyle(); }
    DSizeType getMusicIndex() const { return musicIndex_; }
	void setMusicIndex(DSizeType index) { musicIndex_ = index; }

signals:
    void clicked(DSizeType index);

protected:
    DSizeType musicIndex_;
	State state_;
	QColor color_;
};

#endif // BASEMUSICBUTTON_H
