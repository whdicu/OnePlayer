/*
* date:2022-12-12
* author: FL
* purpose: ncm file to mp3
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "aes.h"
#include "cJSON.h"
#include "ncmToMp3.h"


#ifdef WIN32
#include<Windows.h>
//返回转换好的字符串指针
unsigned char* utf8ToGbk(unsigned char*src,int len)
{
	wchar_t* tmp = (wchar_t*)malloc(sizeof(wchar_t) * len+2);
	unsigned char* newSrc = (unsigned char*)malloc(sizeof(unsigned char) * len + 2);
	
	MultiByteToWideChar(CP_UTF8, 0, src, -1, tmp, len);	//转为unicode
	WideCharToMultiByte(CP_ACP, 0, tmp, -1, newSrc, len+2, NULL,NULL); //转gbk
	
	return newSrc;
}
#endif



void swap(unsigned char* a, unsigned char* b)
{
	unsigned char t = *a;
	*a = *b;
	*b = t;
}

//用key生成S盒
/*
* s: s盒
* key: 密钥
* len: 密钥长度
*/
void rc4Init(unsigned char* s, const unsigned char* key, int len)
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
		swap(s + i, s + j);
	}
}
//针对NCM文件的解密
//异或关系
/*
* s: s盒
* data: 要加密或者解密的数据
* len: data的长度
*/
void rc4PRGA(unsigned char* s, unsigned char* data, int len)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int idx = 0;
	for (idx = 0; idx < len; idx++)
	{
		i = (idx + 1) % 256;
		j = (i + s[i]) % 256;
		k = (s[i] + s[j]) % 256;
		data[idx] ^= s[k];  //异或
	}
}

//base64 解码
/*
* code: 要解码的数据
*/
unsigned char* base64_decode(unsigned char* code, int len, int* actLen)
{
	//根据base64表，以字符找到对应的十进制数据  
	int table[] = { 0,0,0,0,0,0,0,0,0,0,0,0,
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
	long str_len;
	unsigned char* res;
	int i, j;

	//计算解码后的字符串长度  
	//判断编码后的字符串后是否有=
	if (strstr(code, "=="))
		str_len = len / 4 * 3 - 2;
	else if (strstr(code, "="))
		str_len = len / 4 * 3 - 1;
	else
		str_len = len / 4 * 3;

	*actLen = str_len;
	res = malloc(sizeof(unsigned char) * str_len + 1);
	res[str_len] = '\0';

	//以4个字符为一位进行解码  
	for (i = 0, j = 0; i < len - 2; j += 3, i += 4)
	{
		res[j] = ((unsigned char)table[code[i]]) << 2 | (((unsigned char)table[code[i + 1]]) >> 4); 
		res[j + 1] = (((unsigned char)table[code[i + 1]]) << 4) | (((unsigned char)table[code[i + 2]]) >> 2);  
		res[j + 2] = (((unsigned char)table[code[i + 2]]) << 6) | ((unsigned char)table[code[i + 3]]); 
	}
	return res;

}

// 原来的代码，现在没用到
void readFileData(const char* fileName, const char* outputDir, char** jsonStr)
{
	FILE* f = NULL;
	fopen_s(&f, fileName, "rb");
	if (!f)
	{
		printf("No such file: %s\n", fileName);
		return;
	}
	
	unsigned char buf[16];
	int len=0;
	int i = 0;

	unsigned char meta_key[] = { 0x23,0x31,0x34,0x6C,0x6A,0x6B,0x5F,0x21,0x5C,0x5D,0x26,0x30,0x55,0x3C,0x27,0x28 };
	unsigned char core_key[] = { 0x68,0x7A,0x48,0x52,0x41,0x6D,0x73,0x6F,0x35,0x6B,0x49,0x6E,0x62,0x61,0x78,0x57 };
	
	fseek(f, 10, SEEK_CUR); //f从当前位置移动10个字节
	fread(buf, 1, 4, f);    //读取rc4 key 的长度

	len = (buf[3] << 8 | buf[2]) << 16 | (buf[1] << 8 | buf[0]);
	unsigned char* rc4Key= (unsigned char*)malloc(sizeof(unsigned char) * len);
	fread(rc4Key, 1, len, f);   //读取rc4数据

	//解密rc4密钥
	for (i = 0; i < len; i++)
	{
		rc4Key[i] ^= 0x64;
	}
	
	struct AES_ctx ctx;	
	AES_init_ctx(&ctx, core_key);	//使用core_key密钥
	int packSize = len / 16;	//采用的是AES-ECB加密方式，和Pkcs7padding填充
	for (i = 0; i < packSize; i++)
	{
		AES_ECB_decrypt(&ctx, &rc4Key[i * 16]);
	}
	int pad = rc4Key[len - 1];	//获取填充的长度
	rc4Key[len - pad] = '\0';	//去除填充的部分，得到RC4密钥


	fread(buf, 1, 4, f);    //读取Music Info 长度数据
	len = ((buf[3] << 8 | buf[2]) << 16) | (buf[1] << 8 | buf[0]);
	unsigned char* meta = (unsigned char*)malloc(sizeof(unsigned char) * len);
	fread(meta, 1, len, f); //读取Music Info数据
	//解析Music info信息
	for (i = 0; i < len; i++)
	{
		meta[i] ^= 0x63;
	}
	int act = 0;
	unsigned char* data = base64_decode(&meta[22], len - 22, &act);	//base64解码
	AES_init_ctx(&ctx, meta_key);	//AES解密
	packSize = act / 16;
	for (i = 0; i < packSize; i++)
	{
		AES_ECB_decrypt(&ctx, &data[i * 16]);
	}
	pad = data[act - 1];
	data[act - pad] = '\0';	//去除填充部分
	unsigned char* newData = data;
#ifdef WIN32
	
	newData = utf8ToGbk(data, strlen(data));
	
#endif
	
	cJSON* cjson = cJSON_Parse(&newData[6]);	//json解析，获取格式和名字等
	if (cjson == NULL)
	{
		printf("cjson parse failed\n");
		return;
	}
	char* tempJsonStr = cJSON_Print(cjson);
	*jsonStr = (char*)malloc(strlen(tempJsonStr) + 1);
	strcpy_s(*jsonStr, strlen(tempJsonStr) + 1, tempJsonStr);
	printf("%s\n", *jsonStr);	//输出json



	fseek(f, 9, SEEK_CUR);  //从当前位置跳过9个字节
	fread(buf, 1, 4, f);    //读取图片大小
	len = (buf[3] << 8 | buf[2]) << 16 | (buf[1] << 8 | buf[0]);
	unsigned char* img = (unsigned char*)malloc(sizeof(unsigned char) * len);
	fread(img, 1, len, f);  //读取图片数据



	int offset= 1024 * 1024 * 10;    //10MB 音乐数据一般比较大一次读入10MB
	int total = 0;
	int reSize = offset;
	unsigned char* musicData = (unsigned char*)malloc(offset); //10m
	
	while (!feof(f))
	{
		len = fread(musicData+total, 1, offset, f);	//每次读取10M
		total += len;
		reSize += offset;
	    musicData=realloc(musicData,reSize);	//扩容
	}
	
	unsigned char sBox[256] = { 0 };	//s盒
	rc4Init(sBox, &rc4Key[17], strlen(&rc4Key[17]));	//用rC4密钥进行初始化s盒
	rc4PRGA(sBox, musicData, total);	//解密

	//拼接文件名(artist + music name+format)
	char* musicName = cJSON_GetObjectItem(cjson, "musicName")->valuestring;
	cJSON* sub = cJSON_GetObjectItem(cjson, "artist");
	char* artist = cJSON_GetArrayItem(cJSON_GetArrayItem(sub, 0),0)->valuestring;
	char* format = cJSON_GetObjectItem(cjson, "format")->valuestring;
	//char* saveFileName = (char*)malloc(strlen(musicName) + strlen(artist) + strlen(format)+5);

	// 提取歌曲文件名
	char* filename = malloc(strlen(fileName) + 1);
	if (filename == NULL)
	{
		printf("内存分配失败！\n");
		return;
	}
	strcpy_s(filename, strlen(fileName) + 1, fileName);

	char* fn = strrchr(filename, '/'); // 找到最后一个'/'
	if (fn == NULL)
	{
		fn = strrchr(filename, '\\'); // 找到最后一个'\\'
		if (fn == NULL)
		{
			printf("无法提取文件名，使用默认文件名:ncm转换后的音乐\n");
			fn = "/ncm转换后的音乐";
		}
	}

	char* dot = strrchr(fn, '.'); // 找到文件名中最后一个'.'
	if (dot != NULL)
	{
		*dot = '\0'; // 将文件名中最后一个'.'替换为'\0'，即将文件名截断
	}
	printf("提取出的文件名是: %s\n", fn);

	size_t ss = strlen(outputDir) + strlen(fn) + strlen(format) + 5;
	char* saveFileName = (char*)malloc(ss);
	if (saveFileName)
	{
		if (0 != strcpy_s(saveFileName, ss, outputDir))
			return;
		size_t dest_len = strlen(saveFileName);
		if (0 != strcat_s(saveFileName + dest_len, ss, fn))
			return;
		dest_len = strlen(saveFileName);
		if (0 != strcat_s(saveFileName + dest_len, ss, "."))
			return;
		dest_len = strlen(saveFileName);
		if (0 != strcat_s(saveFileName + dest_len, ss, format))
			return;
		//sprintf_s(saveFileName, sizeof(saveFileName), "%s - %s.%s", artist, musicName, format);
		printf(saveFileName);
		FILE* fo = NULL;
		fopen_s(&fo, saveFileName, "wb");
		if (fo == NULL)
		{
			printf("The fileName - '%s' is invalid in this system\n", saveFileName);
		}
		else
		{
			fwrite(musicData, 1, total, fo);
			fclose(fo);
		}
	}
	else
	{
		printf("saveFileName is nullptr!");
	}
	
#ifdef WIN32
	free(newData);
#endif
	free(data);
	free(meta);
	free(img);
	free(musicData);
	fclose(f);
	free(saveFileName);
	free(filename);
}

int getFileData(const char* fileName, struct CMusicData* structMusicData)
{
	FILE* f = NULL;
	fopen_s(&f, fileName, "rb");
	if (!f)
	{
		printf("No such file: %s\n", fileName);
		return -1;
	}

	unsigned char buf[16];
	int len = 0;
	int i = 0;

	unsigned char meta_key[] = { 0x23,0x31,0x34,0x6C,0x6A,0x6B,0x5F,0x21,0x5C,0x5D,0x26,0x30,0x55,0x3C,0x27,0x28 };
	unsigned char core_key[] = { 0x68,0x7A,0x48,0x52,0x41,0x6D,0x73,0x6F,0x35,0x6B,0x49,0x6E,0x62,0x61,0x78,0x57 };

	fseek(f, 10, SEEK_CUR); //f从当前位置移动10个字节
	fread(buf, 1, 4, f);    //读取rc4 key 的长度

	len = (buf[3] << 8 | buf[2]) << 16 | (buf[1] << 8 | buf[0]);
	unsigned char* rc4Key = (unsigned char*)malloc(sizeof(unsigned char) * len);
	fread(rc4Key, 1, len, f);   //读取rc4数据

	//解密rc4密钥
	for (i = 0; i < len; i++)
	{
		rc4Key[i] ^= 0x64;
	}

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, core_key);	//使用core_key密钥
	int packSize = len / 16;	//采用的是AES-ECB加密方式，和Pkcs7padding填充
	for (i = 0; i < packSize; i++)
	{
		AES_ECB_decrypt(&ctx, &rc4Key[i * 16]);
	}
	int pad = rc4Key[len - 1];	//获取填充的长度
	rc4Key[len - pad] = '\0';	//去除填充的部分，得到RC4密钥


	fread(buf, 1, 4, f);    //读取Music Info 长度数据
	len = ((buf[3] << 8 | buf[2]) << 16) | (buf[1] << 8 | buf[0]);
	unsigned char* meta = (unsigned char*)malloc(sizeof(unsigned char) * len);
	fread(meta, 1, len, f); //读取Music Info数据
	//解析Music info信息
	for (i = 0; i < len; i++)
	{
		meta[i] ^= 0x63;
	}
	int act = 0;
	unsigned char* data = base64_decode(&meta[22], len - 22, &act);	//base64解码
	AES_init_ctx(&ctx, meta_key);	//AES解密
	packSize = act / 16;
	for (i = 0; i < packSize; i++)
	{
		AES_ECB_decrypt(&ctx, &data[i * 16]);
	}
	pad = data[act - 1];
	data[act - pad] = '\0';	//去除填充部分
	unsigned char* newData = data;
#ifdef WIN32

	newData = utf8ToGbk(data, strlen(data));

#endif

	cJSON* cjson = cJSON_Parse(&newData[6]);	//json解析，获取格式和名字等
	if (cjson != NULL)
	{
		char* tempJsonStr = cJSON_Print(cjson);
		structMusicData->jsonStr = (char*)malloc(strlen(tempJsonStr) + 1);
		if (NULL != structMusicData->jsonStr)
		{
			strcpy_s(structMusicData->jsonStr, strlen(tempJsonStr) + 1, tempJsonStr);
			//printf("%s\n", structMusicData->jsonStr);	//输出json
		}
		cJSON_Delete(cjson);
		free(tempJsonStr);
	}

	fseek(f, 9, SEEK_CUR);  //从当前位置跳过9个字节
	fread(buf, 1, 4, f);    //读取图片大小
	len = (buf[3] << 8 | buf[2]) << 16 | (buf[1] << 8 | buf[0]);
	structMusicData->imgData = (unsigned char*)malloc(sizeof(unsigned char) * len);
	if (NULL != structMusicData->imgData)
	{
		fread(structMusicData->imgData, 1, len, f);  //读取图片数据
		structMusicData->imgDataSize = len;
	}

	int offset = 1024 * 1024 * 10;    //10MB 音乐数据一般比较大一次读入10MB
	structMusicData->dataSize = 0;
	int reSize = offset;
	structMusicData->data = (unsigned char*)malloc(offset); //10m

	while (!feof(f))
	{
		len = fread(structMusicData->data + structMusicData->dataSize, 1, offset, f);	//每次读取10M
		structMusicData->dataSize += len;
		reSize += offset;
		structMusicData->data = realloc(structMusicData->data, reSize);	//扩容
	}

	unsigned char sBox[256] = { 0 };	//s盒
	rc4Init(sBox, &rc4Key[17], strlen(&rc4Key[17]));	//用rC4密钥进行初始化s盒
	rc4PRGA(sBox, structMusicData->data, structMusicData->dataSize);	//解密

#ifdef WIN32
	free(newData);
#endif
	free(rc4Key);
	free(meta);
	free(data);
	//free(img);
	//free(musicData);
	fclose(f);

	return 0;
}
