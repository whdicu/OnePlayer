#ifndef ONLINEHANDLER_H
#define ONLINEHANDLER_H

#include "HDBase/DList.hpp"
#include "OnePlayerStruct.h"
#include <QImage>
#include <QList>
#include <QString>
#include <QUrl>

class OnlineHandler
{
public:
    static OnlineHandler* getInstance();

    // 获取一个网页的html
    QByteArray get_html(const QUrl& url);

    // 根据关键词搜索音乐，返回所有音乐的QStringList
    DList<OnlineMusicInfo> search_online_music(const QString& word);

    // 根据一个音乐页面的链接，获取音乐文件的链接
    void get_music_info(OnlineMusicInfo& music);

    // 根据图片链接获取图片
    QImage get_image(const QString& url);

    // 根据音乐链接获取音乐
    QByteArray get_music(const QUrl& url) {return get_html(url);}

private:
    OnlineHandler();
    ~OnlineHandler();
};

#endif // ONLINEHANDLER_H
