#ifndef LOCALMUSICBUTTON_H
#define LOCALMUSICBUTTON_H

#include "basemusicbutton.h"
#include "OnePlayerStruct.h"
#include <QUrl>
#include <QWidget>

namespace Ui {
class LocalMusicButton;
}

class LocalMusicButton : public BaseMusicButton
{
    Q_OBJECT

public:
    LocalMusicButton(const QString& str, QWidget *parent = nullptr);
    ~LocalMusicButton();
	virtual void refreshStyle() override;

private slots:
    void on_btn_name_clicked() { emit clicked(musicIndex_); }

private:
    Ui::LocalMusicButton *ui;
};

#endif // LOCALMUSICBUTTON_H
