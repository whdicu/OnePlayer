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
	NeteaseInfo& getNeteaseInfo() { return setting_.neteaseInfo; }
	void save() { writeAll(); }

	// 添加一些音乐到新的播放列表
    void addPlayList(const QString& playListName, const DVector<QString>& list);

	// 添加一些音乐到当前播放列表，会排除已存在的歌曲
	void add2CurrentPlayList(const DVector<QString>& list);

    // 从当前播放列表中移除歌曲
	void removeMusicFromCurrentPlayList(DSizeType musicIndex);

	// 查看播放列表是否存在
	bool isPlayListExists(const QString& playListName);

	// 当前没有任何播放列表
	bool notExistPlayList();

    // 获取当前歌单下的歌曲，随机播放时 不是 返回随机播放歌曲Index列表
	const DVector<QString> currentPlayList();
	DSizeType getCurrentPlayListSize() { return currentPlayList().size(); }
	void clearRandomPlayList();
	void insertToRandomPlayList(DSizeType musicIndex);

    DSizeType nextMusicIndex();
	// 随机播放时根据randomIndex获取musicIndex，其他播放模式直接使用musicIndex_
	QString currentMusicUrl();
    DSizeType previousMusicIndex();

    void setMusicIndex(DSizeType musicIndex);
    DSizeType getMusicIndex() { return musicIndex_; }

	// 在非随机播放模式下，菜单中点击了下一首播放
	void setNextIndexTemp(DSizeType nextIndexTemp) { nextIndexTemp_ = nextIndexTemp; }

	void plusRandomIndex() { ++randomIndex_; }

	void refreshPlayList() { readPlayList(); }

	void setCurrentNeteaseSongsInfo(const DVector<NeteaseSongInfo>& info) { currentNeteaseSongsInfo_ = info; }
	NeteaseSongInfo getNeteaseSongInfo(DSizeType index);
	NeteaseSongInfo getNeteaseSongInfo(dint64 id);

	// 根据网易云音乐的id获取它在播放列表中的下标
	DSizeType getNeteaseSongIndex(dint64 id);

signals:
    void sigMusicIndexChanged(DSizeType oldIndex, DSizeType newIndex);

private:
    SettingHandler();
    ~SettingHandler() = default;

    const DVector<QString> getPlayList(const QString& name) { return setting_.playListMap.value(name); }

    // 检查播放链表的名字，如果有重复则在尾部添加 "_数字"
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
    DVector<DSizeType> randomIndexList_;

	DVector<NeteaseSongInfo> currentNeteaseSongsInfo_;  // 当前正在播放的网易云音乐列表的所有歌曲信息
};

#define SETTING_HANDLER SettingHandler::getInstance()

#endif // SETTINGHANDLER_H
