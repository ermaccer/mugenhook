#pragma once
#include <vector>

struct eStageAnnouncerEntry {
	int StageID;
	int Group;
	int Index;

};


class eStageAnnouncer {
public:
	static std::vector<eStageAnnouncerEntry> m_vStageEntries;

	static void   Init();
	static void   ReadFile(const char* file);
	static int    FindStageData(int id);
};