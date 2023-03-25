#ifndef ONLINEMUSICBUTTON_H
#define ONLINEMUSICBUTTON_H

#include "basemusicbutton.h"
#include "onlinehandler.h"
#include <QWidget>

namespace Ui {
class OnlineMusicButton;
}

static const QString ONLINE_NORMAL_STYLE = "QPushButton {color: #5c5c66;background-color: rgba(182, 209, 200, 0.25);border-radius: 0px;} #btn_name{text-align : left;padding-left: 10px;border-top-left-radius: 15px;border-bottom-left-radius: 15px;}#btn_singer{text-align : left;}#btn_download{color: #1473e6;border-top-right-radius: 15px;border-bottom-right-radius: 15px;}";
static const QString ONLINE_PLAYING_STYLE = "QPushButton {color: #5c5c66;background-color: rgb(182, 209, 200);border-radius: 0px;} #btn_name{text-align : left;padding-left: 10px;border-top-left-radius: 15px;border-bottom-left-radius: 15px;}#btn_singer{text-align : left;}#btn_download{color: #1473e6;border-top-right-radius: 15px;border-bottom-right-radius: 15px;}";
static const QString ONLINE_HOVER_STYLE = "QPushButton {color: #5c5c66;background-color: rgba(182, 209, 200, 0.5);border-radius: 0px;} #btn_name{text-align : left;padding-left: 10px;border-top-left-radius: 15px;border-bottom-left-radius: 15px;}#btn_singer{text-align : left;}#btn_download{color: #1473e6;border-top-right-radius: 15px;border-bottom-right-radius: 15px;}";

class OnlineMusicButton : public BaseMusicButton
{
    Q_OBJECT

public:
    OnlineMusicButton(const MusicInfo& music, QWidget *parent = nullptr);
    ~OnlineMusicButton();
    virtual const QUrl& get_url() const {return music_info_.absolute_url_;}
    virtual QString get_filename() const {return "filename";}  // 没用
    virtual void setNormalStyle() {is_playing_ = false; setStyleSheet(ONLINE_NORMAL_STYLE);}
    virtual void setPlayingStyle() {is_playing_ = true; setStyleSheet(ONLINE_PLAYING_STYLE);};
    MusicInfo& get_info() {return music_info_;}

private slots:
    void on_btn_name_clicked() {emit clicked();}
    void on_btn_singer_clicked() {emit clicked();}
    void on_btn_download_clicked() {emit download_clicked();}

signals:
    void download_clicked();

private:
    void enterEvent(QEnterEvent *);
    void leaveEvent(QEvent *);

    Ui::OnlineMusicButton *ui;
    MusicInfo music_info_;
    bool is_playing_;
};

#endif // ONLINEMUSICBUTTON_H
