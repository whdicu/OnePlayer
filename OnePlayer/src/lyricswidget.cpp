#include "lyricswidget.h"
#include <QDebug>
#include "ui_lyricswidget.h"

LyricsWidget::LyricsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LyricsWidget)
    , lyrics_(DList<Lyric>())
    , now_it_(lyrics_.begin())
    , old_duration_(0)
{
    ui->setupUi(this);
}

LyricsWidget::~LyricsWidget()
{
    delete ui;
}

void LyricsWidget::set_lyrics(const DList<Lyric>& lyrics)
{
    lyrics_ = lyrics;
    ui->label1->setText("");
    ui->label2->setText("");
    if (lyrics_.isEmpty())
        ui->label3->setText("");
    else
        ui->label3->setText(lyrics_.first().text);
    now_it_ = lyrics_.begin();
    old_duration_ = 0;
}

bool LyricsWidget::set_duration(qint64 duration)
{
    if (now_it_ == lyrics_.end())
        return false;

//    qDebug() << duration << now_it_->duration;

    // 说明往前拖进度条了
    if (duration < old_duration_)
    {
        QString s1 = "";
        QString s2 = "";
        for (DList<Lyric>::iterator it = lyrics_.begin(); it != lyrics_.end(); ++it)
        {
            if (duration < it->duration)
            {
                now_it_ = it;
                break;
            }
            s1 = s2;
            s2 = it->text;
        }

        ui->label1->setText(s1);
        ui->label2->setText(s2);
        ui->label3->setText(now_it_->text);
    }
    else if (duration >= now_it_->duration)
    {
        ui->label1->setText(ui->label2->text());
        ui->label2->setText(ui->label3->text());

        old_duration_ = now_it_->duration;
        ++now_it_;
        if (now_it_ == lyrics_.end())
        {
            ui->label3->setText("");
            return false;
        }
        else
            ui->label3->setText(now_it_->text);
    }
    return true;
}

void LyricsWidget::set_color(bool is_dark)
{
    if (is_dark)
    {
        setStyleSheet("#label1,#label3 {color: rgba(92, 92, 102, 0.4);} #label2 {color: #5c5c66;}");
    }
    else
    {
        setStyleSheet("#label1,#label3 {color: rgba(182, 209, 200, 0.4);} #label2 {color: #b6d1c8;}");
    }
}
