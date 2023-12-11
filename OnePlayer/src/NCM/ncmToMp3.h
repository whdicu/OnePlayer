#ifdef WIN32
#include<Windows.h>

//返回转换好的字符串指针
unsigned char* utf8ToGbk(unsigned char* src, int len);
#endif

struct CMusicData
{
	unsigned char* data;
	int dataSize;
	char* jsonStr;
};

void swap(unsigned char* a, unsigned char* b);

void rc4Init(unsigned char* s, const unsigned char* key, int len);

void rc4PRGA(unsigned char* s, unsigned char* data, int len);

unsigned char* base64_decode(unsigned char* code, int len, int* actLen);

void readFileData(const char* fileName, const char* outputDir, char** jsonStr);

int getFileData(const char* fileName, struct CMusicData* structMusicData);
