#include "settinghandler.h"
#include "HDCore/HD2QT.hpp"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRandomGenerator64>
#endif


QByteArray readFile(const QString& filePath)
{
    QByteArray data;
    QFile file(filePath);
    if (!file.exists())
    {
        qWarning() << filePath << "not exist";
        bool ret = file.open(QIODevice::WriteOnly);
        if (!ret)
            qWarning() << filePath << "create failed";
        else
            file.close();
        return data;
    }

    bool ok = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!ok)
    {
        qWarning() << "File" << filePath << "open failed";
        return data;
    }

    data = file.readAll();
    file.close();
    return data;
}

static SettingHandler* setting_handler = nullptr;
SettingHandler* SettingHandler::getInstance()
{
    if (nullptr == setting_handler)
        setting_handler = new SettingHandler();
    return setting_handler;
}

void SettingHandler::addPlayList(const QString& name, const DList<QUrl>& list)
{
    QString uniqueName = checkPlayListName(name);
    setting_.playListMap.insert(uniqueName, list);
    writeAll();
}

const DList<QUrl> SettingHandler::currentPlayList()
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
			DSizeType newIndex = qrand() % currentPlayList().size();
#endif
			randomIndexList_.pushBack(newIndex);
		}
        setMusicIndex(randomIndexList_.at(randomIndex_));
		break;
	}
	default:
	{
		if (getMusicIndex() >= currentPlayList().size() - 1)
            setMusicIndex(0);
        else
            setMusicIndex(getMusicIndex() + 1);
		break;
	}
	}

    return getMusicIndex();
}

QUrl SettingHandler::currentMusicUrl()
{
	if ((setting_.playListName.isEmpty() || setting_.playListName == "Null")
		&& !setting_.playListMap.isEmpty())
	{
		setting_.playListName = setting_.playListMap.begin().key();
	}

    if (getMusicIndex() >= currentPlayList().size())
        return QUrl();

    return currentPlayList().at(getMusicIndex());
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
            DSizeType newIndex = qrand() % currentPlayList().size();
#endif
			randomIndexList_.pushFront(newIndex);
		}
        setMusicIndex(randomIndexList_.at(randomIndex_));
		break;
	}
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

void SettingHandler::setMusicIndex(DSizeType index)
{
    if (musicIndex_ != index)
    {
        DSizeType oldIndex = musicIndex_;
        musicIndex_ = index;
        emit sigMusicIndexChanged(oldIndex, index);
    }
}

SettingHandler::SettingHandler()
    : QObject(nullptr)
    , setting_(SettingStruct())
    , musicIndex_(0)
{
    readAll();
}

QString SettingHandler::checkPlayListName(const QString& name)
{
    if (setting_.playListMap.contains(name))
        return checkPlayListName(name + "_新");
    return name;
}

void SettingHandler::readAll()
{
    QString strFile = QCoreApplication::applicationDirPath();
    QString filePath = "/config/Setting.json";
    strFile += filePath;

    QByteArray data = readFile(strFile);
    if (0 == data.size())
    {
        qWarning() << "File" << filePath << "is empty";
        return;
    }

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    if (QJsonParseError::NoError != parseError.error)
    {
        qWarning() << "File" << filePath << "analyze failed";
        return;
    }

    QJsonObject obj = jsonDoc.object();

    setting_.playMode = (PLAY_MODE)obj["playMode"].toInt();
    setting_.musicDir = obj["musicDir"].toString();
    setting_.volume = obj["volume"].toDouble();
    setting_.playListName = obj["playListName"].toString();
    setMusicIndex(obj["musicIndex"].toVariant().toULongLong());
    setting_.musicPosition = obj["musicPosition"].toVariant().toLongLong();
    setting_.playerMode = (PLAYER_MODE)obj["playerMode"].toInt();
    setting_.downloadDir = obj["downloadDir"].toString();

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
    wholeObject.insert("volume", setting_.volume);
    wholeObject.insert("playListName", setting_.playListName);
    wholeObject.insert("musicIndex", QString::number(getMusicIndex()));
    wholeObject.insert("musicPosition", setting_.musicPosition);
    wholeObject.insert("playerMode", setting_.playerMode);
    wholeObject.insert("downloadDir", setting_.downloadDir);
    
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
        qWarning() << "File" << strFile << "open failed!";
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
    directory.setNameFilters({"*.oned"});
    QStringList fileList = directory.entryList();
    for (const QString& fileName : fileList)
    {
        QString str = readFile(basePath + fileName).trimmed();
        if (str.isEmpty())
        {
            //QFile().remove(basePath + fileName);
            qWarning() << "File is empty! File:" << (basePath + fileName);
            continue;
        }
        QStringList strList = str.split('\n');
        QString playListName = fileName.mid(0, fileName.indexOf('.'));
		DList<QUrl> ret;
		for (const QString& str : strList)
		{
			ret.pushBack(QUrl::fromLocalFile(str));
		}
        setting_.playListMap.insert(playListName, ret);
    }
    
    //QJsonObject playListObject = obj["playList"].toObject();
    //QStringList playList = playListObject.keys();
    //for (const QString& listName : playList)
    //{
    //    DList<QString> oneList;
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
    basePath += "/config/play_lists/%1.oned";

    for (auto it = setting_.playListMap.cbegin(); it != setting_.playListMap.cend(); ++it)
    {
        // 如果路径中有不存在的文件夹则创建
        QString strFile = basePath.arg(it.key());
        QFileInfo fileInfo(strFile);
        QDir().mkpath(fileInfo.absolutePath());

        QFile file(strFile);
        bool ok = file.open(QIODevice::WriteOnly);
		if (!ok)
		{
			qWarning() << "File" << strFile << "open failed!";
			continue;
		}

		for (const QUrl& path : it.value())
		{
			file.write(path.toLocalFile().toUtf8() + '\n');
		}
		file.close();
    }
    
}
