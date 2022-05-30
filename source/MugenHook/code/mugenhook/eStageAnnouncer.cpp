#include "eStageAnnouncer.h"
#include "..\core\eSettingsManager.h"
#include "..\core\eLog.h"
std::vector<eStageAnnouncerEntry> eStageAnnouncer::m_vStageEntries;
void eStageAnnouncer::Init()
{
	if (eSettingsManager::bHookStageAnnouncer)
		ReadFile("cfg\\stageAnn.dat");
}

void eStageAnnouncer::ReadFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		eLog::PushMessage(__FUNCTION__, "Failed! Tried to open: %s\n", file);
		eLog::PushError();
	}

	if (pFile)
	{
		eLog::PushMessage(__FUNCTION__, "Success! Reading: %s\n", file);

		char szLine[2048];
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			// check if comment or empty line
			if (szLine[0] == ';' || szLine[0] == '#' || szLine[0] == '\n')
				continue;

			int iStageID = 0;
			if (sscanf(szLine, "%d", &iStageID) == 1)
			{
				int Group = 0;
				int Index = 0;

					sscanf(szLine, "%d %d %d", &iStageID, &Group,&Index);

				// create entry
				eStageAnnouncerEntry stg = { iStageID,Group,Index };
				m_vStageEntries.push_back(stg);

			}
		}
		eLog::PushMessage(__FUNCTION__, "Read %d entries\n", m_vStageEntries.size());
		fclose(pFile);
	}
}

int eStageAnnouncer::FindStageData(int id)
{
	int iFind = 0;
	for (unsigned int i = 0; i < m_vStageEntries.size(); i++)
	{
		if (m_vStageEntries[i].StageID == id) {
			iFind = i;
			break;
		}
	}
	return iFind;
}
