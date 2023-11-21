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

// 主界面大小
const static int MAIN_WIDGET_WIDTH = 920;
const static int MAIN_WIDGET_HEIGHT = 520;

// 右侧按钮框的位置大小
const static int MORE_BTN_WIDGET_X = 420;
const static int MORE_BTN_WIDGET_Y = 0;
const static int MORE_BTN_WIDGET_WIDTH = 40;
const static int MORE_BTN_WIDGET_HEIGHT = 400;

// 音乐按钮控件大小
const static int STACKED_MUSIC_BTN_WIDTH = 400;

// 主界面显示隐藏的过渡动画时间
const static int MAIN_WIDGET_ANIMATION_TIME = 250;

// 右侧按钮框的过渡动画Widget
const static int MORE_BTN_WIDGET_ANIMATION_TIME = 250;

// 左侧图片widget的过渡动画时间
const static int MUSIC_INFO_WIDGET_ANIMATION_TIME = 150;

// 搜索框过渡动画时间
const static int SEARCH_EDIT_ANIMATION_TIME = 200;
