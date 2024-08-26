#include "settinghandler.h"
#include "HDCore/HD2QT.hpp"
#include <mutex>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRandomGenerator64>
#else
#include <QTime>
#endif


QByteArray readFile(const QString& filePath)
{
    QByteArray data;
    QFile file(filePath);
    if (!file.exists())
    {
		DWarning << filePath << "not exist";
        bool ret = file.open(QIODevice::WriteOnly);
        if (!ret)
			DWarning << filePath << "create failed";
        else
            file.close();
        return data;
    }

    bool ok = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!ok)
    {
		DWarning << "File" << filePath << "open failed";
        return data;
    }

    data = file.readAll();
    file.close();
    return data;
}

const DVector<float> SettingHandler::VOLUME_VEC = 
{ 0.0f, 0.05f, 0.06f, 0.072, 0.0864f, 0.1037f, 0.1244f, 0.1493f, 0.1792f, 0.2150f, 
0.2580f, 0.3096f, 0.3715f, 0.4458f, 0.5350f, 0.6420f, 0.7704f, 0.9244f, 1.0f};

static std::once_flag onceFlag;
static SettingHandler* setting_handler = nullptr;
SettingHandler* SettingHandler::getInstance()
{
	std::call_once(onceFlag, [] { setting_handler = new SettingHandler; });
    return setting_handler;
}

void SettingHandler::addPlayList(const QString& playListName, const DVector<QString>& list, bool coverOld)
{
    QString uniqueName = coverOld ? playListName : checkPlayListName(playListName);
    setting_.playListMap.insert(uniqueName, list);
    writeAll();
}

void SettingHandler::add2CurrentPlayList(const DVector<QString>& list)
{
	if (!setting_.playListMap.contains(setting_.playListName))
	{
		DWarning << "PlayList" << setting_.playListName << "is not exist, add musics faild!";
		return;
	}

	// 去除播放列表中已存在的歌曲
	DVector<QString> onlyList;
	for (const QString& url : list)
	{
		if (setting_.playListMap.value(setting_.playListName).contains(url))
			continue;
		onlyList.pushBack(url);
	}

	setting_.playListMap[setting_.playListName].insert(0, onlyList);

    // 如果是随机播放，则修改下标列表中的值
    if (setting_.playMode == RANDOM)
    {
        for (DSizeType& index : randomIndexList_)
        {
            index += onlyList.size();
        }
    }
}

void SettingHandler::removeMusicFromCurrentPlayList(DSizeType musicIndex)
{
	// 如果是随机播放，则修改下标列表中的值
	if (setting_.playMode == RANDOM)
	{
		DSizeType removeCount = randomIndexList_.removeAll(musicIndex);

		for (DSizeType& index : randomIndexList_)
		{
			if (index > musicIndex)
				--index;
		}
	}

	removeMusic(setting_.playListName, musicIndex);
}

bool SettingHandler::isPlayListExists(const QString& playListName)
{
	return setting_.playListMap.contains(playListName);
}

bool SettingHandler::notExistPlayList()
{
	return setting_.playListMap.isEmpty();
}

const DVector<QString> SettingHandler::currentPlayList()
{
	if ((setting_.playListName.isEmpty() || setting_.playListName == "Null")
		&& !setting_.playListMap.isEmpty())
	{
		setting_.playListName = setting_.playListMap.begin().key();
	}
    return getPlayList(setting_.playListName);
}

void SettingHandler::clearRandomPlayList()
{
    randomIndex_ = 0;
    randomIndexList_.clear();
    randomIndexList_.pushBack(getMusicIndex());
}

void SettingHandler::insertToRandomPlayList(DSizeType musicIndex)
{
	if (randomIndex_ > randomIndexList_.size())
		randomIndex_ = randomIndexList_.size();

	if (randomIndex_ >= randomIndexList_.size() - 1)
	{
		randomIndexList_.pushBack(musicIndex);
		randomIndex_ = randomIndexList_.size() - 2;
	}
    else if (randomIndexList_.at(randomIndex_ + 1) != musicIndex)
    {
		// 下一首已经是它了，就不再添加
        randomIndexList_.insert(randomIndex_ + 1, musicIndex);
    }
}

DSizeType SettingHandler::nextMusicIndex()
{
	switch (setting_.playMode)
	{
	case RANDOM:
	{
		++randomIndex_;
		if (randomIndex_ >= randomIndexList_.size())
		{
			randomIndex_ = randomIndexList_.size();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            DSizeType newIndex = QRandomGenerator64::global()->bounded(0ull, currentPlayList().size());
#else
			qsrand(QTime::currentTime().msec());
			DSizeType newIndex = qrand() % currentPlayList().size();
#endif
			randomIndexList_.pushBack(newIndex);
		}
        setMusicIndex(randomIndexList_.at(randomIndex_));
		break;
	}
    case ONE_AGAIN:
        break;
	default:
	{
		// 在菜单中点过下一首播放
		if (nextIndexTemp_ != -1)
		{
			setMusicIndex(nextIndexTemp_);
			nextIndexTemp_ = -1;
			break;
		}

		if (getMusicIndex() >= currentPlayList().size() - 1)
            setMusicIndex(0);
        else
            setMusicIndex(getMusicIndex() + 1);
		break;
	}
	}

    return getMusicIndex();
}

QString SettingHandler::currentMusicUrl()
{
	if ((setting_.playListName.isEmpty() || setting_.playListName == "Null")
		&& !setting_.playListMap.isEmpty())
	{
		setting_.playListName = setting_.playListMap.begin().key();
	}

	switch (setting_.playMode)
	{
	case RANDOM:
		setMusicIndex(randomIndexList_.at(randomIndex_));
		break;
	default:
		break;
	}
    if (musicIndex_ >= currentPlayList().size())
        return "";

    return currentPlayList().at(musicIndex_);
}

DSizeType SettingHandler::previousMusicIndex()
{
	switch (setting_.playMode)
	{
	case RANDOM:
	{
		--randomIndex_;
		if (randomIndex_ >= randomIndexList_.size())
		{
			randomIndex_ = 0;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            DSizeType newIndex = QRandomGenerator64::global()->bounded(0ull, currentPlayList().size());
#else
			qsrand(QTime::currentTime().msec());
            DSizeType newIndex = qrand() % currentPlayList().size();
#endif
			randomIndexList_.pushFront(newIndex);
		}
        setMusicIndex(randomIndexList_.at(randomIndex_));
		break;
	}
    case ONE_AGAIN:
        break;
	default:
	{
		if (getMusicIndex() == 0)
            setMusicIndex(currentPlayList().size() - 1);
        else
            setMusicIndex(getMusicIndex() - 1);
		break;
	}
	}
    
    return getMusicIndex();
}

void SettingHandler::setMusicIndex(DSizeType musicIndex)
{
	if (musicIndex_ == musicIndex)
		return;

	DSizeType oldIndex = musicIndex_;
	musicIndex_ = musicIndex;
	emit sigMusicIndexChanged(oldIndex, musicIndex);
}

NeteaseSongInfo SettingHandler::getNeteaseSongInfo(DSizeType index)
{
	if (index >= currentNeteaseSongsInfo_.size())
		return NeteaseSongInfo();
	else
		return currentNeteaseSongsInfo_.at(index);
}

NeteaseSongInfo SettingHandler::getNeteaseSongInfo(dint64 id)
{
	auto it = std::find_if(currentNeteaseSongsInfo_.begin(), currentNeteaseSongsInfo_.end(), [id](const NeteaseSongInfo& info)
	{
		return info.id == id;
	});

	if (it == currentNeteaseSongsInfo_.end())
		return NeteaseSongInfo();
	else
		return *it;
}

DSizeType SettingHandler::getNeteaseSongIndex(dint64 id)
{
	auto it = std::find_if(currentNeteaseSongsInfo_.begin(), currentNeteaseSongsInfo_.end(), [id](const NeteaseSongInfo& info)
	{
		return info.id == id;
	});
	return it - currentNeteaseSongsInfo_.begin();
}

float SettingHandler::upVolume()
{
	if (setting_.volumeIndex + 1 < VOLUME_VEC.size())
		++setting_.volumeIndex;
	return VOLUME_VEC.at(setting_.volumeIndex);
}

float SettingHandler::downVolume()
{
	if (setting_.volumeIndex > 0)
		--setting_.volumeIndex;
	return VOLUME_VEC.at(setting_.volumeIndex);
}

float SettingHandler::getVolume()
{
	return VOLUME_VEC.at(setting_.volumeIndex);
}

SettingHandler::SettingHandler()
    : QObject(nullptr)
    , setting_(SettingStruct())
    , musicIndex_(0)
	, nextIndexTemp_(-1)
{
    readAll();
}

QString SettingHandler::checkPlayListName(const QString& name)
{
	if (setting_.playListMap.contains(name))
	{
		int i = name.lastIndexOf('_');
		int num = name.mid(i + 1).toInt();
		QString newName = name.mid(0, i) + '_' + QString::number(num+1);
		return checkPlayListName(newName);
	}
        
    return name;
}

void SettingHandler::removeMusic(const QString& playListName, DSizeType musicIndex)
{
    if (!setting_.playListMap.contains(playListName))
    {
        DWarning << "play list not exist:" << playListName;
        return;
    }

    setting_.playListMap[playListName].removeAt(musicIndex);
}

void SettingHandler::readAll()
{
    QString strFile = QCoreApplication::applicationDirPath();
    QString filePath = "/config/Setting.json";
    strFile += filePath;

    QByteArray data = readFile(strFile);
    if (0 == data.size())
    {
        DWarning << "File" << filePath << "is empty";
        return;
    }

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    if (QJsonParseError::NoError != parseError.error)
    {
        DWarning << "File" << filePath << "analyze failed";
        return;
    }

    QJsonObject obj = jsonDoc.object();

    setting_.playMode = (PLAY_MODE)obj["playMode"].toInt();
    setting_.musicDir = obj["musicDir"].toString();
    setting_.volumeIndex = obj["volumeIndex"].toVariant().toLongLong();
    setting_.playListName = obj["playListName"].toString();
    setMusicIndex(obj["musicIndex"].toVariant().toULongLong());
    setting_.musicPosition = obj["musicPosition"].toVariant().toLongLong();
    setting_.playerMode = (PLAYER_MODE)obj["playerMode"].toInt();
    setting_.bgMode = (BG_MODE)obj["bgMode"].toInt(FULL_WIDGET);  // 默认背景图全屏
    setting_.downloadDir = obj["downloadDir"].toString();
    setting_.showLyric = obj["showLyric"].toBool();

	// 读取网易云相关信息
	QJsonObject neteaseObject = obj["neteaseInfo"].toObject();
	setting_.neteaseInfo.cookie = neteaseObject.value("cookie").toString();
	setting_.neteaseInfo.token = neteaseObject.value("token").toString();
	setting_.neteaseInfo.userId = neteaseObject.value("userId").toVariant().toLongLong();
	setting_.neteaseInfo.avatarUrl = neteaseObject.value("avatarUrl").toString();
	setting_.neteaseInfo.nickname = neteaseObject.value("nickname").toString();

    // 读取播放列表
    readPlayList();
}

void SettingHandler::writeAll()
{
    QString strFile = QCoreApplication::applicationDirPath();
    strFile += "/config/Setting.json";

    QJsonObject wholeObject;

    wholeObject.insert("playMode", setting_.playMode);
    wholeObject.insert("musicDir", setting_.musicDir);
    wholeObject.insert("volumeIndex", QString::number(setting_.volumeIndex));
	if (TEMP_PLAY_LIST_NAME != setting_.playListName)
		wholeObject.insert("playListName", setting_.playListName);
    wholeObject.insert("musicIndex", QString::number(getMusicIndex()));
    wholeObject.insert("musicPosition", setting_.musicPosition);
    wholeObject.insert("playerMode", setting_.playerMode);
    wholeObject.insert("bgMode", setting_.bgMode);
    wholeObject.insert("downloadDir", setting_.downloadDir);
    wholeObject.insert("showLyric", setting_.showLyric);
    
	// 写入网易云相关信息
	QJsonObject neteaseObject;
	neteaseObject.insert("userId", setting_.neteaseInfo.userId);
	neteaseObject.insert("cookie", setting_.neteaseInfo.cookie);
	neteaseObject.insert("token", setting_.neteaseInfo.token);
	neteaseObject.insert("avatarUrl", setting_.neteaseInfo.avatarUrl);
	neteaseObject.insert("nickname", setting_.neteaseInfo.nickname);
	wholeObject.insert("neteaseInfo", neteaseObject);

    writePlayList();

    // 如果路径中有不存在的文件夹则创建
    QFileInfo fileInfo(strFile);
    QDir().mkpath(fileInfo.absolutePath());

    QJsonDocument doc(wholeObject);
    QByteArray data = doc.toJson();
    QFile file(strFile);
    bool ok = file.open(QIODevice::WriteOnly);
    if (ok)
    {
        file.write(data);
        file.close();
    }
    else
    {
        DWarning << "File" << strFile << "open failed!";
    }
}

void SettingHandler::readPlayList()
{
    setting_.playListMap.clear();
    QString basePath = QCoreApplication::applicationDirPath();
    basePath += "/config/play_lists/";

    QDir directory(basePath);
    if (!directory.exists())
    {
        QFileInfo fileInfo(basePath);
        directory.mkpath(fileInfo.absolutePath());
    }

    directory.setFilter(QDir::Files | QDir::NoDotAndDotDot); // 只过滤文件，不包括"."和".."
    directory.setNameFilters({"*" + PLF_FORMAT });
    QStringList fileList = directory.entryList();
    for (const QString& fileName : fileList)
    {
        QString str = readFile(basePath + fileName).trimmed();
        QStringList strList = str.split('\n');
        QString playListName = fileName.mid(0, fileName.indexOf('.'));

		// 与临时播放列表同名的，或者是以网易云播放列表名称前缀开头的，不添加
		if (playListName == TEMP_PLAY_LIST_NAME
			|| playListName.startsWith(NETEASE_PLAY_LIST_PREFIX))
		{
			DDebug << "Play list name is the same as TEMP_PLAY_LIST! It would be ignore.";
			continue;
		}

		qint64 i1 = QDateTime::currentMSecsSinceEpoch();
		DVector<QString> ret;
		for (const QString& str : strList)
		{
			// 排除不支持的格式
			if (!TYPE_LIST.contains(str.section('.', -1)))
				continue;

			ret.pushBack(str);
		}
        setting_.playListMap.insert(playListName, ret);

		qint64 i2 = QDateTime::currentMSecsSinceEpoch();
		qDebug() << (i2 - i1);
    }
    
    //QJsonObject playListObject = obj["playList"].toObject();
    //QStringList playList = playListObject.keys();
    //for (const QString& listName : playList)
    //{
    //    DVector<QString> oneList;
    //    QJsonArray musicArray = playListObject[listName].toArray();
    //    for (const QJsonValue& musicUrl : musicArray)
    //    {
    //        oneList.pushBack(musicUrl.toString());
    //    }
    //    setting_.playListMap.insert(listName, oneList);
    //}
}

void SettingHandler::writePlayList()
{
    QString basePath = QCoreApplication::applicationDirPath();
    basePath += "/config/play_lists/%1" + PLF_FORMAT;

    for (auto it = setting_.playListMap.cbegin(); it != setting_.playListMap.cend(); ++it)
    {
		// 临时播放列表和网易云的播放列表不写入
		if (TEMP_PLAY_LIST_NAME == it.key()
			|| it.key().startsWith(NETEASE_PLAY_LIST_PREFIX))
			continue;

        // 如果路径中有不存在的文件夹则创建
        QString strFile = basePath.arg(it.key());
        QFileInfo fileInfo(strFile);
        QDir().mkpath(fileInfo.absolutePath());

        QFile file(strFile);
        bool ok = file.open(QIODevice::WriteOnly);
		if (!ok)
		{
			DWarning << "File" << strFile << "open failed!";
			continue;
		}

		for (const QString& path : it.value())
		{
			file.write(path.toUtf8() + '\n');
		}
		file.close();
    }
    
}
