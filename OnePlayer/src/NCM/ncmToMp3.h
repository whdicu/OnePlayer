#pragma once
#include <QByteArray>
#include <QJsonObject>
#include <QString>

// NCM 文件解析后的数据（全部使用 Qt 容器，自动管理内存）
struct NCMMusicInfo
{
	QByteArray  data;     // 音频数据（已 RC4 解密）
	QByteArray  imgData;  // 封面图片
	QJsonObject json;     // 元数据（已解析的 JSON 对象）
};

// 解析 NCM 文件，结果写入 structMusicData，成功返回 0
int getFileData(const QString& fileName, NCMMusicInfo* structMusicData);
