#ifndef SETTINGHANDLER_H
#define SETTINGHANDLER_H

#include <QFile>
#include "widget.h"

enum PLAYER_MODE
{
    LOCAL,
    MYSITE,
    ONLINE
};

class SettingHandler
{
public:
    static SettingHandler* getInstance();
    void init_setting();
    PLAY_MODE get_old_mode() {return old_mode_;}
    void set_old_mode(PLAY_MODE mode) {old_mode_ = mode; write_all();}
    QString get_music_dir() {return music_dir_;}
    void set_music_dir(const QString& music_dir) {music_dir_ = music_dir; write_all();}
    const QUrl& get_last_music() {return last_music_;}
    void set_last_music(const QUrl& music) {last_music_ = music; write_all();}
    float get_volume() {return volume_;}
    void set_volume(float volume) {volume_ = volume; write_all();}
    qint64 get_music_position() {return music_position_;}
    void set_music_position(qint64 pos) {music_position_ = pos; write_all();}
    PLAYER_MODE get_player_mode() {return player_mode_;}
    void set_player_mode(PLAYER_MODE player_mode) {player_mode_ = player_mode; write_all();}

private:
    SettingHandler();
    ~SettingHandler() = default;
    QMap<QString, QStringList> read_all();
    void write_all();

    QFile file;
    PLAY_MODE old_mode_;
    QString music_dir_;
    QUrl last_music_;
    float volume_;
    qint64 music_position_;
    PLAYER_MODE player_mode_;
};

#define SETTING_HANDLER SettingHandler::getInstance()

#endif // SETTINGHANDLER_H
