#ifndef LOCALMUSICBUTTON_H
#define LOCALMUSICBUTTON_H

#include "basemusicbutton.h"
#include "OnePlayerStruct.h"
#include <QUrl>
#include <QWidget>

namespace Ui {
class LocalMusicButton;
}

class QEnterEvent;

class LocalMusicButton : public BaseMusicButton
{
    Q_OBJECT

public:
    LocalMusicButton(const QUrl& url, QWidget *parent = nullptr);
    ~LocalMusicButton();
	virtual void refreshStyle() override;

	// 设置封面图片，由MusicImageLoader在工作线程读完后回调（GUI线程内调用）
	void setMusicCover(const QImage& image);

private slots:
    void on_btn_name_clicked() { emit clicked(musicIndex_); }

private:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	void enterEvent(QEnterEvent*);
#else
	void enterEvent(QEvent*);
#endif
    void leaveEvent(QEvent *event) override;

    Ui::LocalMusicButton *ui;
};

#endif // LOCALMUSICBUTTON_H
