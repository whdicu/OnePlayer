#include "musicbutton.h"

MusicButton::MusicButton(const QUrl& url, QWidget *parent)
    : QPushButton(parent), url_(url)
{

}
