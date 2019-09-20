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
	std::vector<eAirData> vAnimData;
};

class eAirReader {
	std::vector<eAirEntry> vAnimations;
	FILE* pFile;
	std::string strName;
	int  iAnimCount;
public:
	void Open(const char* name);
	void ReadData();
	eAirEntry GetAnimation(int id);
	int  GetAnimCount();
	void PrintAnimID();
	bool CheckName(std::string name);

};

extern std::vector<eAirReader> eAirManager;
eAirEntry GlobalGetAnimation(int id);
eAirReader GetAIRFromName(std::string name);
int GlobalGetAnimCount();
