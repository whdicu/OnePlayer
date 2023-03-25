#ifndef ONLINEHANDLER_H
#define ONLINEHANDLER_H

#include "qurl.h"
#include <QList>
#include <QString>
#include <QUrl>

struct MusicInfo
{
    MusicInfo(const QString& name = QString(), const QString& singer = QString(), const QString& url = QString()
            , const QString& image_url = QString(), const QUrl& absolute_url = QUrl(), const QStringList& lyrics = QStringList())
        : name_(name), singer_(singer), url_(url)
        , image_url_(image_url), absolute_url_(absolute_url), lyrics_(lyrics) {}
    bool isEmpty() { return name_.isEmpty() && singer_.isEmpty() && url_.isEmpty(); }

    QString name_;
    QString singer_;
    QString url_;
    QString image_url_;
    QUrl absolute_url_;
    QStringList lyrics_;
};

class OnlineHandler
{
public:
    static OnlineHandler* getInstance();

    // 获取一个网页的html
    QByteArray get_html(const QUrl& url);

    // 根据关键词搜索音乐，返回所有音乐的QStringList
    QList<MusicInfo> search_online_music(const QString& word);

    // 根据一个音乐页面的链接，获取音乐文件的链接
    void get_music_info(MusicInfo& music);

private:
    OnlineHandler();
    ~OnlineHandler();
};

#endif // ONLINEHANDLER_H
