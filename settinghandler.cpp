#include "settinghandler.h"
#include <QApplication>

static SettingHandler* setting_handler = nullptr;
SettingHandler *SettingHandler::getInstance()
{
    if (nullptr == setting_handler)
        setting_handler = new SettingHandler();
    return setting_handler;
}

SettingHandler::SettingHandler()
    : file(QApplication::applicationDirPath() + "/setting.xq")
    , old_mode_(AGAIN)
    , music_dir_(QString())
    , last_music_(QUrl())
    , volume_(0.5f)
    , music_position_(0)
    , is_online_(false)
{
    init_setting();
}

QMap<QString, QStringList> SettingHandler::read_all()
{
    QMap<QString, QStringList> ret;
    if (! file.exists())
    {
        file.open(QIODevice::WriteOnly);
        file.close();
    }

    auto ok = file.open(QIODevice::ReadOnly);
    QString word = "";
    if (ok)
        word = file.readAll();

    file.close();
    auto setting_list = word.split('\n');
    for (const auto &str: setting_list)
    {
        auto one = str.split(',');

        if (one.size() > 1)
            ret.insert(one.at(0), QStringList(++one.begin(), one.end()));
    }
    return ret;
}

void SettingHandler::write_all()
{
    file.open(QIODevice::WriteOnly);
    file.write(("music_dir," + music_dir_ + "\n").toUtf8());
    file.write(("old_mode," + QString::number(old_mode_) + "\n").toUtf8());
    file.write(("last_music," + last_music_.path() + "\n").toUtf8());
    file.write(("volume," + QString::number(volume_) + "\n").toUtf8());
    file.write(("music_position," + QString::number(music_position_) + "\n").toUtf8());
    file.write(("is_online," + QString::number(is_online_) + "\n").toUtf8());
    file.close();
}

void SettingHandler::init_setting()
{
    auto setting_map = read_all();

    auto v1 = setting_map.value("music_dir", {});
    if (! v1.empty())
        set_music_dir(*v1.begin());

    auto v2 = setting_map.value("old_mode", {});
    if (! v2.empty())
        set_old_mode(static_cast<PLAY_MODE> (v2.begin()->toInt()));

    auto v3 = setting_map.value("last_music", {});
    if (! v3.empty())
        set_last_music(QUrl::fromLocalFile(*v3.begin()));

    auto v4 = setting_map.value("volume", {});
    if (! v4.empty())
        set_volume(v4.begin()->toFloat());

    auto v5 = setting_map.value("music_position", {});
    if (! v5.isEmpty())
        set_music_position(v5.begin()->toInt());

    auto v6 = setting_map.value("is_online", {});
    if (! v6.isEmpty())
        set_is_online(v6.begin()->toInt());
}
