#ifndef _ALEXIS_NCM_2_STD_API_H_
#define _ALEXIS_NCM_2_STD_API_H_

#ifdef NCM2STD_EXPORTS
#define NCM2STD_API __declspec(dllexport)
#else
#define NCM2STD_API __declspec(dllimport)
#endif

#include <string>
#include <vector>

class NCM2STD_API MusicBlock {
public:
	MusicBlock();
	MusicBlock(void* obj, bool isfree = false);
	void initBlock(void* obj, bool isfree = false);
public:
	std::string name;
	std::string artist;
	std::string format;
	std::string albumpic;
	std::string error;
	std::vector<unsigned char> data;
	bool inited{ false };
};

class NCM2STD_API CNcm2Std {
public:
	static bool Conver(const std::string &_url, MusicBlock &_music);
};

#endif // !_ALEXIS_NCM_2_STD_API_H_
