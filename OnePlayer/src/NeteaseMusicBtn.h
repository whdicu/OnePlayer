#ifndef NETEASEMUSICBTN_H
#define NETEASEMUSICBTN_H

#include "basemusicbutton.h"
#include "OnePlayerStruct.h"
#include "onlinehandler.h"


namespace Ui {
class NeteaseMusicBtn;
}

class NeteaseMusicBtn : public BaseMusicButton
{
    Q_OBJECT

public:
	NeteaseMusicBtn(const NeteaseSongInfo& info, QWidget *parent = nullptr);
    ~NeteaseMusicBtn();
	virtual void refreshStyle() override;

private slots:
    void on_btn_name_clicked() {emit clicked(info_.id);}
    void on_btn_singer_clicked() {emit clicked(info_.id);}
    void on_btn_space_clicked() {emit clicked(info_.id);}

private:
    void enterEvent(QEnterEvent *);
    void leaveEvent(QEvent *);

    Ui::NeteaseMusicBtn* ui;
    bool isPlaying_;
	NeteaseSongInfo info_;
};

#endif // NETEASEMUSICBTN_H
