#pragma once
#include "HDBase/DVector.hpp"
#include "HDMemory/DSharedPointer.hpp"
#include <QDateTime>
#include <QEasingCurve>
#include <QImage>
#include <QMap>
#include <QString>
#include <QUrl>

#define LOG_HEAD QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]") << __FUNCTION__
#define DWarning (qWarning().noquote() << LOG_HEAD)
#define DDebug (qDebug().noquote() << LOG_HEAD)


#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRandomGenerator64>
#define GET_PLAY_STATE player_->playbackState()
#define SET_VOLUME(v) audioOutput_->setVolume((v));
const static QStringList TYPE_LIST = { "mp3", "flac", "wav", "ogg", "acc", "m4a", "ncm"};
#else
#include <QMediaPlaylist>
#define GET_PLAY_STATE player_->state()
#define SET_VOLUME(v) player_->setVolume((v) * 100);
const static QStringList TYPE_LIST = { "mp3", "wav", "ogg", "acc", "ncm"};  // 5.9.4无法播放flac m4a
#endif

using SharedImage = DSharedPointer<QImage>;
class Lyric;
using Lyrics = DVector<Lyric>;

const static QString PLF_FORMAT = ".oned";  // PlayList file format
const static QString TEMP_PLAY_LIST_NAME = "TEMP_PLAY_LIST";  // 临时播放列表名，用来存右键打开的歌曲
const static QString NETEASE_PLAY_LIST_PREFIX = "_NETEASE_";  // 网易云的播放列表名称前缀
const static QString IP = "47.113.231.74";
const static int PORT = 9002;

// 主界面大小
const static int MAIN_WIDGET_WIDTH = 920;
const static int MAIN_WIDGET_HEIGHT = 520;

// 左侧音乐信息Widget的大小
const static int MUSIC_INFO_WIDGET_WIDTH = 400;
const static int MUSIC_INFO_WIDGET_HEIGHT = 400;

// 右侧按钮框的位置大小
const static int MORE_BTN_WIDGET_X = 420;
const static int MORE_BTN_WIDGET_Y = 0;
const static int MORE_BTN_WIDGET_WIDTH = 40;
const static int MORE_BTN_WIDGET_HEIGHT = 420;

// 音乐按钮控件大小
const static int STACKED_MUSIC_BTN_WIDTH = 400;
const static int STACKED_MUSIC_BTN_HEIGHT = 355;


/****************************** 时间 ******************************/
// 主界面显示隐藏的过渡动画时间;
const static int MAIN_WIDGET_ANIMATION_TIME = 250;
const static QEasingCurve MAIN_WIDGET_SHOW_EASING = QEasingCurve::Linear;
const static QEasingCurve MAIN_WIDGET_HIDE_EASING = QEasingCurve::Linear;

// 右侧按钮框的过渡动画Widget
const static int MORE_BTN_WIDGET_ANIMATION_TIME = 300;

// 左侧图片widget的过渡动画
const static int MUSIC_INFO_WIDGET_ANIMATION_TIME = 150;
const static QEasingCurve MUSIC_INFO_WIDGET_SHOW_EASING = QEasingCurve::InQuad;
const static QEasingCurve MUSIC_INFO_WIDGET_HIDE_EASING = QEasingCurve::OutQuad;
const static QEasingCurve STACKED_MUSIC_BTN_BIG = MUSIC_INFO_WIDGET_SHOW_EASING;
const static QEasingCurve STACKED_MUSIC_BTN_SMALL = MUSIC_INFO_WIDGET_HIDE_EASING;

// 搜索框过渡动画时间
const static int SEARCH_EDIT_ANIMATION_TIME = 200;

const static QStringList BUTTON_MENU_STR_LIST =
{
	QString("下一首播放"),
	QString("打开文件所在位置"),
	QString("从列表中移除"),
	QString("删除")
};


/****************************** 样式 ******************************/
// 本地音乐按钮样式
static const QString LOCAL_NORMAL_STYLE = "QPushButton { color: #5c5c66; background-color: rgba(182, 209, 200, 0.25); border-radius: 20px; padding-left: 10px; padding-right: 10px; } QPushButton:hover {background-color: rgba(182, 209, 200, 0.5);}";
static const QString LOCAL_PLAYING_STYLE = "QPushButton { color: #5c5c66; background-color: rgb(182, 209, 200);border-radius: 20px; padding-left: 10px; padding-right: 10px; }";
static const QString LOCAL_SELECT_STYLE = "QPushButton { color: #5c5c66; background-color: rgba(182, 209, 200, 0.5); border-radius: 20px; padding-left: 10px; padding-right: 10px; }";

// 网易云音乐按钮样式
static const QString NETEASE_NORMAL_STYLE =
"QPushButton { \
	color: #5c5c66; \
	background-color: transparent; \
	text-align : left; \
} #widget { \
	background-color: rgba(182, 209, 200, 0.25); \
	border-radius: 20px; \
} #btn_name{ \
	padding-left: 15px; \
}";


static const QString NETEASE_PLAYING_STYLE =
"QPushButton { \
	color: #5c5c66; \
	background-color: transparent; \
	text-align : left; \
} #widget { \
	background-color: rgb(182, 209, 200); \
	border-radius: 20px; \
} #btn_name{ \
	padding-left: 15px; \
}";

static const QString NETEASE_HOVER_STYLE =
"QPushButton { \
	color: #5c5c66; \
	background-color: transparent; \
	text-align : left; \
} #widget { \
	background-color: rgba(182, 209, 200, 0.5); \
	border-radius: 20px; \
} #btn_name{ \
	padding-left: 15px; \
}";

// 在线音乐按钮样式
static const QString ONLINE_NORMAL_STYLE =
"QPushButton { \
	color: #5c5c66; \
	background-color: rgba(182, 209, 200, 0.25); \
	border-radius: 0px; \
} #btn_name { \
	text-align: left; \
	padding-left: 10px; \
	border-top-left-radius: 20px; \
	border-bottom-left-radius: 20px; \
} #btn_singer { \
	text-align: left; \
} #btn_download { \
	color: #1473e6; \
	border-top-right-radius: 20px; \
	border-bottom-right-radius: 20px; \
}";

static const QString ONLINE_PLAYING_STYLE =
"QPushButton { \
	color: #5c5c66; \
	background-color: rgb(182, 209, 200); \
	border-radius: 0px; \
} #btn_name { \
	text-align: left; \
	padding-left: 10px; \
	border-top-left-radius: 20px; \
	border-bottom-left-radius: 20px; \
} #btn_singer { \
	text-align: left; \
} #btn_download { \
	color: #1473e6; \
	border-top-right-radius: 20px; \
	border-bottom-right-radius: 20px; \
}";

static const QString ONLINE_HOVER_STYLE = 
"QPushButton { \
	color: #5c5c66; \
	background-color: rgba(182, 209, 200, 0.5); \
	border-radius: 0px; \
} #btn_name { \
	text-align: left; \
	padding-left: 10px; \
	border-top-left-radius: 20px; \
	border-bottom-left-radius: 20px; \
} #btn_singer { \
	text-align: left; \
} #btn_download { \
	color: #1473e6; \
	border-top-right-radius: 20px; \
	border-bottom-right-radius: 20px; \
}";

static const QString PLAY_LIST_NORMAL_STYLE =
"#widget_bg { \
	background-color: rgba(182, 209, 200, 0.25); \
	border-radius: 20px; \
}";

static const QString PLAY_LIST_PLAYING_STYLE =
"#widget_bg { \
	background-color: rgb(182, 209, 200); \
	border-radius: 20px; \
}";

static const QString PLAY_LIST_HOVER_STYLE =
"#widget_bg { \
	background-color: rgba(182, 209, 200, 0.5); \
	border-radius: 20px; \
}";

static const QString PLAY_LIST_BTNS_STYLE =
"#btn_name { \
	color: #5c5c66; \
	background-color: transparent; \
	padding-left: 10px; \
	padding-right: 10px; \
} #btn_delete { \
	color: #5c5c66; \
	background-color: transparent; \
}";


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

enum BG_MODE
{
	ONLY_LEFT,
	FULL_WIDGET
};

// 网易云相关设置信息
struct NeteaseInfo
{
	NeteaseInfo() : hasLogin(false) {}

	bool hasLogin;
	dint64 userId;
	QString cookie;
	QString token;
	QString avatarUrl;
	QString nickname;
};

struct NeteasePlayListInfo
{
	NeteasePlayListInfo(const QString& n = QString(), const QString& c = QString(), dint64 i = -1)
		: name(n)
		, coverImgUrl(c)
		, id(i) {}

	QString name;
	QString coverImgUrl;
	dint64 id;
};

struct NeteaseSongInfo
{
	QString name;
	dint64 id;
	QString singer;
	QString album;
	QString picUrl;
};

struct SettingStruct
{
	SettingStruct() : playMode(AGAIN), musicDir(QString()), volume(0.0f)
		, playListName(QString("Null")), musicPosition(0), playerMode(LOCAL)
		, bgMode(ONLY_LEFT), downloadDir(QString()), playListMap(QMap<QString, DVector<QString>>())
		, neteaseInfo() {}

	PLAY_MODE                       playMode;
	QString                         musicDir;
	float                           volume;
	QString                         playListName;
	qint64                          musicPosition;
	PLAYER_MODE                     playerMode;
	BG_MODE							bgMode;
	QString                         downloadDir;
	QMap<QString, DVector<QString>>	playListMap;
	NeteaseInfo						neteaseInfo;
};

struct MusicInfo
{
	MusicInfo() = default;
	MusicInfo(const MusicInfo& info) = default;
	~MusicInfo() = default;
	MusicInfo& operator=(const MusicInfo& info) = default;

	bool operator==(const MusicInfo& info)
	{
		return (title == info.title)
			&& (singers == info.singers)
			&& (album == info.album)
			&& (image == info.image)
			&& (imgIsReady == info.imgIsReady);
	}

	QString title;
	QString singers;
	QString album;
	QImage image;
	bool imgIsReady;  // img是否已经准备好
};

struct Lyric
{
	Lyric(qint64 d = 0, const QString& t = QString()) : duration(d), text(t) {}

	qint64 duration;
	QString text;
};

struct OnlineMusicInfo
{
	OnlineMusicInfo(const QString& name = QString(), const QString& singer = QString(), const QString& url = QString()
		, const QString& image_url = QString(), const QUrl& absolute_url = QUrl(), const DList<Lyric>& lyrics = DList<Lyric>())
		: name_(name), singer_(singer), url_(url)
		, image_url_(image_url), absolute_url_(absolute_url), lyrics_(lyrics) {}
	bool isEmpty() { return name_.isEmpty() && singer_.isEmpty() && url_.isEmpty(); }

	QString name_;
	QString singer_;
	QString url_;
	QString image_url_;
	QUrl absolute_url_;
	DList<Lyric> lyrics_;
};
