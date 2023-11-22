#ifndef SETTINGHANDLER_H
#define SETTINGHANDLER_H

#include "OnePlayerStruct.h"
#include <QFile>
#include <QString>
#include "widget.h"


class SettingHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DSizeType musicIndex_ READ getMusicIndex WRITE setMusicIndex NOTIFY sigMusicIndexChanged)

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

    void setMusicIndex(DSizeType index);
    DSizeType getMusicIndex() { return musicIndex_; }

signals:
    void sigMusicIndexChanged(DSizeType oldIndex, DSizeType newIndex);

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
    DSizeType musicIndex_;  // 当前正在播放的音乐的index

    DSizeType randomIndex_;
    DList<DSizeType> randomIndexList_;
};

#define SETTING_HANDLER SettingHandler::getInstance()

#endif // SETTINGHANDLER_H
