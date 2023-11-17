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
    QUrl                            lastMusic;
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
    DList<QString> getNowPlayList();
    QString nextMusicPath();
    QString nowMusicPath();
    QString previousMusicPath();
    void save() { writeAll(); }

    //PLAY_MODE get_old_mode() {return old_mode_;}
    //void set_old_mode(PLAY_MODE mode) {old_mode_ = mode; write_all();}
    //QString get_music_dir() {return music_dir_;}
    //void set_music_dir(const QString& music_dir) {music_dir_ = music_dir; write_all();}
    //const QUrl& get_last_music() {return last_music_;}
    //void set_last_music(const QUrl& music) {last_music_ = music; write_all();}
    //float get_volume() {return volume_;}
    //void set_volume(float volume) {volume_ = volume; write_all();}
    //qint64 get_music_position() {return music_position_;}
    //void set_music_position(qint64 pos) {music_position_ = pos; write_all();}
    //PLAYER_MODE get_player_mode() {return player_mode_;}
    //void set_player_mode(PLAYER_MODE player_mode) {player_mode_ = player_mode; write_all();}
    //QString get_download_dir() {return download_dir_;}
    //void set_download_dir(const QString& download_dir) {download_dir_ = download_dir; write_all();}

private:
    SettingHandler();
    ~SettingHandler() = default;

    DList<QString> getPlayList(const QString& name) { return setting_.playListMap.value(name); }

    // 检查播放链表的名字，如果有重复则在尾部添加 "_新"
    QString checkPlayListName(const QString& name);

    void readAll();
    void writeAll();

    SettingStruct setting_;
};

#define SETTING_HANDLER SettingHandler::getInstance()

#endif // SETTINGHANDLER_H
