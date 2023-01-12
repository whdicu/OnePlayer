#ifndef SETTINGHANDLER_H
#define SETTINGHANDLER_H

#include <QFile>
#include "widget.h"

class SettingHandler
{
public:
    static void init_setting();
    static PLAY_MODE get_old_mode() {return old_mode;}
    static void set_old_mode(PLAY_MODE mode) {old_mode = mode; write_all();}
    static QString get_music_dir() {return music_dir_;}
    static void set_music_dir(const QString& music_dir) {music_dir_ = music_dir; write_all();}
    static const QUrl& get_last_music() {return last_music;}
    static void set_last_music(const QUrl& music) {last_music = music; write_all();}
    static float get_volume() {return volume_;}
    static void set_volume(float volume) {volume_ = volume; write_all();}
    static qint64 get_music_position() {return music_position_;}
    static void set_music_position(qint64 pos) {music_position_ = pos; write_all();}

private:
    static QMap<QString, QStringList> read_all();
    static void write_all();

    static QFile file;
    static PLAY_MODE old_mode;
    static QString music_dir_;
    static QUrl last_music;
    static float volume_;
    static qint64 music_position_;
};

#endif // SETTINGHANDLER_H
