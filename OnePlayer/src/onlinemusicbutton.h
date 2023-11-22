#ifndef ONLINEMUSICBUTTON_H
#define ONLINEMUSICBUTTON_H

#include "basemusicbutton.h"
#include "OnePlayerStruct.h"
#include "onlinehandler.h"
#include <QWidget>

namespace Ui {
class OnlineMusicButton;
}

class OnlineMusicButton : public BaseMusicButton
{
    Q_OBJECT

public:
    OnlineMusicButton(const OnlineMusicInfo& music, QWidget *parent = nullptr);
    ~OnlineMusicButton();
    virtual void setNormalStyle() {is_playing_ = false; setStyleSheet(ONLINE_NORMAL_STYLE);}
    virtual void setPlayingStyle() {is_playing_ = true; setStyleSheet(ONLINE_PLAYING_STYLE);};
    OnlineMusicInfo& get_info() {return music_info_;}

private slots:
    void on_btn_name_clicked() {emit clicked(-1);}
    void on_btn_singer_clicked() {emit clicked(-1);}
    void on_btn_download_clicked() {emit download_clicked();}

signals:
    void download_clicked();

private:
    void enterEvent(QEnterEvent *);
    void leaveEvent(QEvent *);

    Ui::OnlineMusicButton *ui;
    OnlineMusicInfo music_info_;
    bool is_playing_;
};

#endif // ONLINEMUSICBUTTON_H
