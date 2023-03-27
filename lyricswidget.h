#ifndef LYRICSWIDGET_H
#define LYRICSWIDGET_H

#include "onlinehandler.h"
#include <QWidget>

namespace Ui {
class LyricsWidget;
}

class LyricsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LyricsWidget(QWidget *parent = nullptr);
    ~LyricsWidget();
    void set_lyrics(const DList<Lyric>& lyrics);
    bool set_duration(qint64 duration);  // 到最后了会返回false
    void set_color(bool is_dark);

private:
    Ui::LyricsWidget *ui;
    DList<Lyric> lyrics_;
    DListIterator<Lyric> now_it_;
    qint64 old_duration_;
};

#endif // LYRICSWIDGET_H
