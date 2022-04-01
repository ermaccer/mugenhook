#pragma once
#include <vector>
#include <iostream>

struct eAirData {
	int Group;
	int Index;
	int PosX;
	int PosY;
	int Frametime;
};

struct eAirEntry {
	int AnimID;
	int MaxFrames;
	bool IsLooping;
	int LoopStart;
	std::vector<eAirData> vAnimData;
};

class eAirReader {
	std::vector<eAirEntry> vAnimations;
	FILE* pFile;
	std::string strName;
public:
	int  m_nAnimCount;

	void Open(const char* name);
	void ReadData();
	bool CheckName(std::string name);

	eAirEntry* GetAnimation(int id);

};

extern std::vector<eAirReader> eAirManager;
eAirReader* GetAIRFromName(std::string name);
int GlobalGetAnimCount();

