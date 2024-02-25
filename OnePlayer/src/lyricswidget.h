#ifndef LYRICSWIDGET_H
#define LYRICSWIDGET_H

#include "onlinehandler.h"
#include <QWidget>
#include "HDBase/DVector.hpp"

namespace Ui {
class LyricsWidget;
}

class LyricsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LyricsWidget(QWidget *parent = nullptr);
    ~LyricsWidget();
    void setLyrics(const QString& lyricStr);
	void setLabel1Text(const QString& text);
	void setLabel2Text(const QString& text);
	void setLabel3Text(const QString& text);
    void setPos(qint64 pos);  // 到最后了会返回false
    void setTextColor(bool is_dark);

private:
    Ui::LyricsWidget* ui;
    DVector<Lyric> lyrics_;
	DVector<Lyric>::iterator nowIt_;
    qint64 old_duration_;
};

#endif // LYRICSWIDGET_H
