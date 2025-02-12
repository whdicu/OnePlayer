#include "LyricsWidget.h"
#include <QDebug>
#include "ui_LyricsWidget.h"
#include <QRegularExpressionMatch>

// 歌词应该用4个label
// 加上滚动动画。颜色变淡(透明的)、文字高度压缩

LyricsWidget::LyricsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LyricsWidget)
    , lyrics_(DVector<Lyric>())
    , nowIt_(lyrics_.begin())
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

	if (lyricStr.isEmpty())
		return;
	
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
        
        // 跳过空的歌词
        if (text.isEmpty())
            continue;

		Lyric lrc((minute * 60 + second) * 1000 + msecond, text);
		lyrics_.pushBack(lrc);
	}

	ui->label1->setText("");
	ui->label2->setText("");
	if (lyrics_.isEmpty())
		ui->label3->setText("");
	else
		ui->label3->setText(lyrics_.first().text);
    nowIt_ = lyrics_.begin();
	old_duration_ = 0;
}

void LyricsWidget::setLabel1Text(const QString& text)
{
	ui->label1->setText(text);
}


QString LyricsWidget::getLabel1Text()
{
	return ui->label1->text();
}

void LyricsWidget::setLabel2Text(const QString& text)
{
	ui->label2->setText(text);
}

void LyricsWidget::setLabel3Text(const QString& text)
{
	ui->label3->setText(text);
}

void LyricsWidget::setPos(qint64 pos)
{
	if (lyrics_.isEmpty())
		return;

    auto it = std::upper_bound(lyrics_.begin(), lyrics_.end(), Lyric(pos));

    if (it == nowIt_)
        return;
    nowIt_ = it;

    if (it == lyrics_.begin() || (it - 1 == lyrics_.begin()))
        ui->label1->setText("");
    else
        ui->label1->setText((it - 2)->text);

    if (it == lyrics_.begin())
        ui->label2->setText("");
    else
        ui->label2->setText((it - 1)->text);

    if (it == lyrics_.end())
        ui->label3->setText("");
    else
        ui->label3->setText(it->text);
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
