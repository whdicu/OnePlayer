#ifndef MUSICBUTTON_H
#define MUSICBUTTON_H

#include "basemusicbutton.h"
#include <QUrl>
#include <QWidget>

namespace Ui {
class MusicButton;
}

static const QString NORMAL_STYLE = "QPushButton {color: #5c5c66;background-color: rgba(182, 209, 200, 0.25);border-radius: 15px; padding-left: 10px; padding-right: 10px;} QPushButton:hover {background-color: rgba(182, 209, 200, 0.5);}";
static const QString PLAYING_STYLE = "QPushButton {color: #5c5c66;background-color: rgb(182, 209, 200);border-radius: 15px; padding-left: 10px; padding-right: 10px;}";

class MusicButton : public BaseMusicButton
{
    Q_OBJECT

public:
    MusicButton(const QUrl& url, QWidget *parent = nullptr);
    ~MusicButton();
    virtual const QUrl& get_url() const {return url_;}
    virtual QString get_filename() const {return url_.fileName().section('.', 0, -2);}
    virtual void setNormalStyle() {setStyleSheet(NORMAL_STYLE);}
    virtual void setPlayingStyle() {setStyleSheet(PLAYING_STYLE);};

private slots:
    void on_btn_name_clicked() { emit clicked(musicIndex_); }

private:
    Ui::MusicButton *ui;
    QUrl url_;
};

#endif // MUSICBUTTON_H
