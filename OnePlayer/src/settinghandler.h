#ifndef SETTINGHANDLER_H
#define SETTINGHANDLER_H

#include <QFile>
#include "widget.h"

enum PLAYER_MODE
{
    LOCAL,
    MYSITE,
    ONLINE,
    NETEASE
};

enum PLAY_MODE
{
    AGAIN,
    ONE_AGAIN,
    RANDOM
};

struct SettingStruct
{
    PLAY_MODE                       playMode;
    QString                         musicDir;
    float                           volume;
    QString                         playListName;
    qint64                          musicIndex;
    qint64                          musicPosition;
    PLAYER_MODE                     playerMode;
    QString                         downloadDir;
    QMap<QString, DList<QString>>   playListMap;
};

class SettingHandler
{
public:
    static SettingHandler* getInstance();

    SettingStruct& getStruct() { return setting_; }
    void addPlayList(const QString& name, const DList<QString>& list);

    // 获取当前歌单下的歌曲，随机播放时 不是 返回随机播放歌曲列表
    DList<QString> getNowPlayList();
    void clearRandomPlayList() { randomIndexList_.clear(); }

    QString nextMusicPath();
    QString nowMusicPath();
    QString previousMusicPath();

    void save() { writeAll(); }

private:
    SettingHandler();
    ~SettingHandler() = default;

    DList<QString> getPlayList(const QString& name) { return setting_.playListMap.value(name); }

    // 检查播放链表的名字，如果有重复则在尾部添加 "_新"
    QString checkPlayListName(const QString& name);

    void readAll();
    void writeAll();

    void readPlayList();
    void writePlayList();

    SettingStruct setting_;

    qint64 randomIndex_;
    DList<qint64> randomIndexList_;
};

#define SETTING_HANDLER SettingHandler::getInstance()

#endif // SETTINGHANDLER_H
