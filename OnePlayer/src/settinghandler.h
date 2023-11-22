#ifndef SETTINGHANDLER_H
#define SETTINGHANDLER_H

#include <QFile>
#include <QString>
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
	SettingStruct() : playMode(AGAIN), musicDir(QString()), volume(0.0f)
		, playListName(QString("Null")), musicIndex(0), musicPosition(0), playerMode(LOCAL)
		, downloadDir(QString()), playListMap(QMap<QString, DList<QUrl>>()) {}

    PLAY_MODE                       playMode;
    QString                         musicDir;
    float                           volume;
    QString                         playListName;
    DSizeType                       musicIndex;
    qint64                          musicPosition;
    PLAYER_MODE                     playerMode;
    QString                         downloadDir;
    QMap<QString, DList<QUrl>>   playListMap;
};

class SettingHandler
{
public:
    static SettingHandler* getInstance();
    SettingStruct& getStruct() { return setting_; }
	void save() { writeAll(); }

    void addPlayList(const QString& name, const DList<QUrl>& list);
    // 获取当前歌单下的歌曲，随机播放时 不是 返回随机播放歌曲Index列表
	const DList<QUrl> currentPlayList();
    void clearRandomPlayList();

    DSizeType nextMusicIndex();
	QUrl currentMusicUrl();
    DSizeType previousMusicIndex();

private:
    SettingHandler();
    ~SettingHandler() = default;

    const DList<QUrl> getPlayList(const QString& name) { return setting_.playListMap.value(name); }

    // 检查播放链表的名字，如果有重复则在尾部添加 "_新"
    QString checkPlayListName(const QString& name);

    void readAll();
    void writeAll();

    void readPlayList();
    void writePlayList();

    SettingStruct setting_;

    DSizeType randomIndex_;
    DList<DSizeType> randomIndexList_;
};

#define SETTING_HANDLER SettingHandler::getInstance()

#endif // SETTINGHANDLER_H
