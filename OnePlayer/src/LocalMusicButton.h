#ifndef LOCALMUSICBUTTON_H
#define LOCALMUSICBUTTON_H

#include "basemusicbutton.h"
#include <QUrl>
#include <QWidget>

namespace Ui {
class LocalMusicButton;
}

static const QString NORMAL_STYLE = "QPushButton {color: #5c5c66;background-color: rgba(182, 209, 200, 0.25);border-radius: 15px; padding-left: 10px; padding-right: 10px;} QPushButton:hover {background-color: rgba(182, 209, 200, 0.5);}";
static const QString PLAYING_STYLE = "QPushButton {color: #5c5c66;background-color: rgb(182, 209, 200);border-radius: 15px; padding-left: 10px; padding-right: 10px;}";

class LocalMusicButton : public BaseMusicButton
{
    Q_OBJECT

public:
    LocalMusicButton(const QString& str, QWidget *parent = nullptr);
    ~LocalMusicButton();
    virtual void setNormalStyle() {setStyleSheet(NORMAL_STYLE);}
    virtual void setPlayingStyle() {setStyleSheet(PLAYING_STYLE);};

private slots:
    void on_btn_name_clicked() { emit clicked(musicIndex_); }

private:
    Ui::LocalMusicButton *ui;
};

#endif // LOCALMUSICBUTTON_H
