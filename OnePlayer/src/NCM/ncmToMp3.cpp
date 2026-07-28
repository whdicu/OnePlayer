/*
* date:2022-12-12
* author: FL
* purpose: ncm file to mp3
* 重构: 使用 QFile/QByteArray 替代 FILE/malloc/realloc，内存由 Qt 自动管理
*/

#include "ncmToMp3.h"

#include <QFile>
#include <QDebug>
#include <QJsonDocument>

#include <openssl/evp.h>
#include <cstring>

// AES-128-ECB 解密（原地解密，关闭 OpenSSL 自动 padding，由调用方自行处理 PKCS7）
static void aes_128_ecb_decrypt(const unsigned char* key, QByteArray& data)
{
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx) return;

	EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);
	EVP_CIPHER_CTX_set_padding(ctx, 0);

	int outLen = 0;
	EVP_DecryptUpdate(ctx,
		reinterpret_cast<unsigned char*>(data.data()), &outLen,
		reinterpret_cast<const unsigned char*>(data.constData()), data.size());

	EVP_CIPHER_CTX_free(ctx);
}

// 生成 RC4 S 盒
static void rc4Init(unsigned char* s, const unsigned char* key, int len)
{
	int i = 0, j = 0;
	unsigned char T[256] = { 0 };

	for (i = 0; i < 256; i++)
	{
		s[i] = i;
		T[i] = key[i % len];
	}

	for (i = 0; i < 256; i++)
	{
		j = (j + s[i] + T[i]) % 256;
		unsigned char t = s[i];
		s[i] = s[j];
		s[j] = t;
	}
}

// RC4 PRGA（加解密同一函数）
static void rc4PRGA(unsigned char* s, unsigned char* data, int len)
{
	int i = 0, j = 0, k = 0;
	for (int idx = 0; idx < len; idx++)
	{
		i = (idx + 1) % 256;
		j = (i + s[i]) % 256;
		k = (s[i] + s[j]) % 256;
		data[idx] ^= s[k];
	}
}

// base64 解码
static QByteArray base64_decode(const QByteArray& code)
{
	static const int table[] = {
		0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,62,0,0,0,
		63,52,53,54,55,56,57,58,
		59,60,61,0,0,0,0,0,0,0,0,
		1,2,3,4,5,6,7,8,9,10,11,12,
		13,14,15,16,17,18,19,20,21,
		22,23,24,25,0,0,0,0,0,0,26,
		27,28,29,30,31,32,33,34,35,
		36,37,38,39,40,41,42,43,44,
		45,46,47,48,49,50,51
	};

	int len = code.size();
	long str_len;
	if (code.endsWith("=="))
		str_len = len / 4 * 3 - 2;
	else if (code.endsWith("="))
		str_len = len / 4 * 3 - 1;
	else
		str_len = len / 4 * 3;

	QByteArray res(str_len, '\0');
	const unsigned char* p = reinterpret_cast<const unsigned char*>(code.constData());
	unsigned char* out = reinterpret_cast<unsigned char*>(res.data());

	for (int i = 0, j = 0; i < len - 2; j += 3, i += 4)
	{
		out[j]     = (unsigned char)((table[p[i]] << 2) | (table[p[i + 1]] >> 4));
		out[j + 1] = (unsigned char)((table[p[i + 1]] << 4) | (table[p[i + 2]] >> 2));
		out[j + 2] = (unsigned char)((table[p[i + 2]] << 6) | table[p[i + 3]]);
	}
	return res;
}

// 从 QFile 读取 4 字节小端整数
static int readLittleEndianInt32(QFile& f)
{
	unsigned char buf[4];
	f.read(reinterpret_cast<char*>(buf), 4);
	return (buf[3] << 8 | buf[2]) << 16 | (buf[1] << 8 | buf[0]);
}

int getFileData(const QString& fileName, NCMMusicInfo* structMusicData)
{
	QFile f(fileName);
	if (!f.open(QIODevice::ReadOnly))
	{
		qWarning() << "No such file:" << fileName;
		return -1;
	}

	static const unsigned char meta_key[] = { 0x23,0x31,0x34,0x6C,0x6A,0x6B,0x5F,0x21,0x5C,0x5D,0x26,0x30,0x55,0x3C,0x27,0x28 };
	static const unsigned char core_key[] = { 0x68,0x7A,0x48,0x52,0x41,0x6D,0x73,0x6F,0x35,0x6B,0x49,0x6E,0x62,0x61,0x78,0x57 };

	// 跳过 10 字节头部
	f.seek(f.pos() + 10);

	// 读取并解密 rc4 密钥
	int len = readLittleEndianInt32(f);
	QByteArray rc4Key = f.read(len);
	{
		unsigned char* p = reinterpret_cast<unsigned char*>(rc4Key.data());
		for (int i = 0; i < len; i++)
			p[i] ^= 0x64;
	}

	aes_128_ecb_decrypt(core_key, rc4Key);
	int pad = (unsigned char)rc4Key[len - 1];  // PKCS7 填充长度
	rc4Key.truncate(len - pad);

	// 读取并解密 Music Info
	len = readLittleEndianInt32(f);
	QByteArray meta = f.read(len);
	{
		unsigned char* p = reinterpret_cast<unsigned char*>(meta.data());
		for (int i = 0; i < len; i++)
			p[i] ^= 0x63;
	}

	// base64 解码（跳过前 22 字节 "163 key(Don't modify):" 前缀）
	QByteArray data = base64_decode(meta.mid(22));
	aes_128_ecb_decrypt(meta_key, data);

	int act = data.size();
	pad = (unsigned char)data[act - 1];
	data.truncate(act - pad);

	// 跳过 "music:" 前缀（6 字节），直接解析为 QJsonObject
	structMusicData->json = QJsonDocument::fromJson(data.mid(6)).object();

	// 跳过 9 字节 CRC，读取封面图片
	f.seek(f.pos() + 9);
	len = readLittleEndianInt32(f);
	structMusicData->imgData = f.read(len);

	// 读取所有剩余音乐数据（QFile::readAll 替代原来的 10MB 循环 realloc）
	structMusicData->data = f.readAll();

	// 用 rc4 密钥（从第 17 字节开始的字符串）解密音乐数据
	unsigned char sBox[256] = { 0 };
	const char* rc4KeyStr = rc4Key.constData() + 17;
	int rc4KeyLen = (int)strlen(rc4KeyStr);
	rc4Init(sBox, reinterpret_cast<const unsigned char*>(rc4KeyStr), rc4KeyLen);
	rc4PRGA(sBox,
		reinterpret_cast<unsigned char*>(structMusicData->data.data()),
		structMusicData->data.size());

	return 0;
}
