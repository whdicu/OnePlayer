#ifndef PLAYLISTBUTTON_H
#define PLAYLISTBUTTON_H

#include "OnePlayerStruct.h"
#include <QUrl>
#include <QWidget>

namespace Ui {
class PlayListButton;
}

class PlayListButton : public QWidget
{
    Q_OBJECT

public:
	PlayListButton(const QString& str, QWidget *parent = nullptr);
    ~PlayListButton();

signals:
	void sigPlayListClicked(const QString& playListName);
	void sigDeleteClicked(const QString& playListName);

private slots:
	void on_btn_name_clikcked();
	void on_btn_delete_clikcked();

private:
    Ui::PlayListButton *ui;
};

#endif // PLAYLISTBUTTON_H
