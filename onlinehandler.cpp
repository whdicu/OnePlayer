#include "onlinehandler.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>

static const QString WEBSITE = "https://www.gequbao.com";

static OnlineHandler* online_handler = nullptr;
OnlineHandler* OnlineHandler::getInstance()
{
    if (nullptr == online_handler)
        online_handler = new OnlineHandler();
    return online_handler;
}

QByteArray OnlineHandler::get_html(const QUrl& url)
{
    QNetworkRequest request(url);
    QNetworkAccessManager* manager(new QNetworkAccessManager());
    manager->get(request);
    QByteArray data;

    QEventLoop loop;
    manager->connect(manager, &QNetworkAccessManager::finished, manager, [&data, &loop](QNetworkReply *reply)
    {
        data = reply->readAll();
        loop.quit();
    });
    loop.exec();

    return data;
}

DList<MusicInfo> OnlineHandler::search_online_music(const QString& word)
{
    QString text = get_html(QString("%1/s/%2").arg(WEBSITE).arg(word));
    static QRegularExpression reg1("<tr>([\\s\\S]*?)</tr>");
    QRegularExpressionMatchIterator it = reg1.globalMatch(text);
    DList<MusicInfo> ret;
    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();
        QString one_info = match.capturedTexts().at(1);

        MusicInfo music;
        static QRegularExpression reg2("class=\"text-primary font-weight-bold\" target=\"_blank\">(.*)");
        auto match2 = reg2.match(one_info);
        if (match2.hasMatch())
        {
            music.name_ = match2.capturedTexts().at(1);
        }

        static QRegularExpression reg3("<td class=\"text-success\">(.*?)</td>");
        auto match3 = reg3.match(one_info);
        if (match3.hasMatch())
        {
            music.singer_ = match3.capturedTexts().at(1);
        }

        static QRegularExpression reg4("<a href=\"(.*?)\" target=\"_blank\">");
        auto match4 = reg4.match(one_info);
        if (match4.hasMatch())
        {
            music.url_ = match4.capturedTexts().at(1);
        }

        if (! music.isEmpty())
            ret.pushBack(music);
    }
    return ret;
}

void OnlineHandler::get_music_info(MusicInfo& music)
{
    QString text = get_html(QString("%1%2").arg(WEBSITE).arg(music.url_));
    static QRegularExpression reg1("const url = '(.*?)'.replace");
    auto match1 = reg1.match(text);
    if (match1.hasMatch())
    {
        music.absolute_url_ = match1.capturedTexts().at(1);
    }

    static QRegularExpression reg2("<img id=\"cover\" src=\"(.*?)\"");
    auto match2 = reg2.match(text);
    if (match2.hasMatch())
    {
        music.image_url_ = match2.capturedTexts().at(1);
    }

    static QRegularExpression reg3("<div class=\"content-lrc mt-1\">([\\s\\S]*?)</div>");
    auto match3 = reg3.match(text);
    if (match3.hasMatch())
    {
        music.lyrics_.clear();
        QStringList temp = match3.capturedTexts().at(1).split("<br />\n");
        for (const QString& one : temp)
        {
            int i1 = one.indexOf('[');
            int i2 = one.indexOf(']');
            if (-1 == i1 || -1 == i2)
                continue;

            QStringList minute_second = one.mid(i1 + 1, i2 - i1 - 1).split(':');
            if (minute_second.size() != 2)
                continue;

            double minutes = minute_second.first().toDouble();
            double seconds = minute_second.back().toDouble();

            qint64 duration = (minutes * 60 + seconds) * qint64(1000);
            Lyric ly(duration, one.mid(i2 + 1));
            music.lyrics_.pushBack(ly);
        }
    }
}

QImage OnlineHandler::get_image(const QString& url)
{
    QNetworkRequest request(url);
    QNetworkAccessManager* manager(new QNetworkAccessManager());
    manager->get(request);
    QByteArray data;

    QEventLoop loop;
    manager->connect(manager, &QNetworkAccessManager::finished, manager, [&data, &loop](QNetworkReply *reply)
    {
        data = reply->readAll();
        loop.quit();
    });
    loop.exec();

    QImage image;
    image.loadFromData(data);
    return image;
}

OnlineHandler::OnlineHandler()
{

}

OnlineHandler::~OnlineHandler()
{

}
