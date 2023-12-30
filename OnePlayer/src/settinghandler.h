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

	// 添加一些音乐到新的播放列表
    void addPlayList(const QString& playListName, const DList<QUrl>& list);

	// 添加一些音乐到当前播放列表，会排除已存在的歌曲
	void addList2CurrentPlayList(const DList<QUrl>& list);

    // 从当前播放列表中移除歌曲
    void removeMusicFromCurrentPlayList(DSizeType musicIndex) { removeMusic(setting_.playListName, musicIndex); }

	// 查看播放列表是否存在
	bool isPlayListExists(const QString& playListName);

	// 当前没有任何播放列表
	bool notExistPlayList();

    // 获取当前歌单下的歌曲，随机播放时 不是 返回随机播放歌曲Index列表
	const DList<QUrl> currentPlayList();
    void clearRandomPlayList();
	void insertToRandomPlayList(DSizeType musicIndex);

    DSizeType nextMusicIndex();
	// 随机播放时根据randomIndex获取musicIndex，其他播放模式直接使用musicIndex_
	QUrl currentMusicUrl();
    DSizeType previousMusicIndex();

    void setMusicIndex(DSizeType musicIndex);
    DSizeType getMusicIndex() { return musicIndex_; }

	// 在非随机播放模式下，菜单中点击了下一首播放
	void setNextIndexTemp(DSizeType nextIndexTemp) { nextIndexTemp_ = nextIndexTemp; }

	void plusRandomIndex() { ++randomIndex_; }

signals:
    void sigMusicIndexChanged(DSizeType oldIndex, DSizeType newIndex);

private:
    SettingHandler();
    ~SettingHandler() = default;

    const DList<QUrl> getPlayList(const QString& name) { return setting_.playListMap.value(name); }

    // 检查播放链表的名字，如果有重复则在尾部添加 "_新"
    QString checkPlayListName(const QString& name);

    // 从播放列表中移除歌曲
    void removeMusic(const QString& playListName, DSizeType musicIndex);

    void readAll();
    void writeAll();

    void readPlayList();
    void writePlayList();

    SettingStruct setting_;
    DSizeType musicIndex_;  // 当前正在播放的音乐的index
	DSizeType nextIndexTemp_;  // 点击菜单中的下一首播放，会将index暂存到这里

    DSizeType randomIndex_;
    DList<DSizeType> randomIndexList_;
};

#define SETTING_HANDLER SettingHandler::getInstance()

#endif // SETTINGHANDLER_H
