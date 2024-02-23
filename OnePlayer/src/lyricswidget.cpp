#include "LyricsWidget.h"
#include <QDebug>
#include "ui_LyricsWidget.h"

// 歌词应该用4个label
// 加上滚动动画。颜色变淡(透明的)、文字高度压缩

LyricsWidget::LyricsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LyricsWidget)
    , lyrics_(DVector<Lyric>())
    , now_it_(lyrics_.begin())
    , old_duration_(0)
{
    ui->setupUi(this);
}

LyricsWidget::~LyricsWidget()
{
    delete ui;
}

void LyricsWidget::setLyrics(const QString& lyricStr)
{
	lyrics_.clear();
	QStringList lyrics = lyricStr.split('\n');
	const static QRegularExpression regex("\\[(.*):(.*)\\.(.*)\\](.*)");
	for (const QString& one : lyrics)
	{
		QRegularExpressionMatch match = regex.match(one);

		if (!match.hasMatch())
			continue;

		int minute = match.captured(1).toInt();
		int second = match.captured(2).toInt();
		int msecond = match.captured(3).toInt();
		QString text = match.captured(4);
		Lyric lrc((minute * 60 + second) * 1000 + msecond, text);
		lyrics_.pushBack(lrc);
	}

	ui->label1->setText("");
	ui->label2->setText("");
	if (lyrics_.isEmpty())
		ui->label3->setText("");
	else
		ui->label3->setText(lyrics_.first().text);
	now_it_ = lyrics_.begin();
	old_duration_ = 0;
}

void LyricsWidget::setLabel1Text(const QString& text)
{
	//ui->label1->setText(text);
}

void LyricsWidget::setLabel2Text(const QString& text)
{
	//ui->label2->setText(text);
}

void LyricsWidget::setLabel3Text(const QString& text)
{
	//ui->label3->setText(text);
}

bool LyricsWidget::setPos(qint64 pos)
{
    if (now_it_ == lyrics_.end())
        return false;

//    qDebug() << duration << now_it_->duration;

    // 说明往前拖进度条了
    if (pos < old_duration_)
    {
        QString s1 = "";
        QString s2 = "";
        for (DVector<Lyric>::iterator it = lyrics_.begin(); it != lyrics_.end(); ++it)
        {
            if (pos < it->duration)
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
    else if (pos >= now_it_->duration)
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

void LyricsWidget::setTextColor(bool is_dark)
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
