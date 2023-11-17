#pragma once
#include <QString>


// 支持播放的文件格式
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRandomGenerator64>
#define GET_PLAY_STATE player_->playbackState()
#define SET_VOLUME(v) audioOutput_->setVolume((v));
const static QStringList TYPE_LIST = { "mp3", "flac", "wav", "ogg", "acc", "m4a" };
#else
#include <QMediaPlaylist>
#define GET_PLAY_STATE player_->state()
#define SET_VOLUME(v) player_->setVolume((v) * 100);
const static QStringList TYPE_LIST = { "mp3", "flac", "wav", "ogg", "acc" };  // 5.9.4无法播放flac m4a
#endif


const static QString IP = "47.113.231.74";
const static int PORT = 9002;

const static int MORE_BTN_WIDGET_ANIMATION_TIME = 250;
const static int TIME250 = 250;
const static int TIME350 = 350;
