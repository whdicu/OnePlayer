#ifndef MUSICBUTTON_H
#define MUSICBUTTON_H

#include <QPushButton>
#include <QUrl>

class MusicButton : public QPushButton
{
    Q_OBJECT
public:
    MusicButton(const QUrl& url, QWidget *parent=nullptr);
    const QUrl& get_url() const {return url_;}
    QString get_filename() const {return url_.fileName().section('.', 0, -2);}

private:
    QUrl url_;
};

#endif // MUSICBUTTON_H
