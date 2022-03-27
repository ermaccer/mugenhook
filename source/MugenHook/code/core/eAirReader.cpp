#include "eAirReader.h"
#include "eLog.h"
#include <string>
#include <algorithm>
#include <Windows.h>
#include "eSettingsManager.h"

std::vector<eAirReader> eAirManager;

void eAirReader::Open(const char * name)
{
	strName = name;
	pFile = fopen(name, "rb");
	if (!pFile)
	{
	    eLog::PushMessage(__FUNCTION__, "Failed! Cannot open %s!\n", name);
		eLog::PushError();
	}


}
void eAirReader::ReadData()
{
	char szLine[1536];
	int i = 0;
	int ID = 0;
	eAirEntry airEntry;
	airEntry.AnimID = 0;
	airEntry.IsLooping = false;
	airEntry.LoopStart = 0;
	airEntry.MaxFrames = 0;
	while (fgets(szLine, sizeof(szLine), pFile))
	{
		if (szLine[0] == '[')
		{
			i++;

			for (int i = 0; szLine[i]; i++) {
				szLine[i] = tolower(szLine[i]);
			}

			if (sscanf(szLine, "[begin action %d]", &ID) == 1)
			{
				while (fgets(szLine, sizeof(szLine), pFile))
				{
					for (int i = 0; szLine[i]; i++) {
						szLine[i] = tolower(szLine[i]);
					}

					if (eSettingsManager::bEnableAnimationLoop)
					{
						if (strncmp(szLine, "loopstart", strlen("loopstart")) == 0)
						{
							airEntry.IsLooping = true;
							airEntry.LoopStart = airEntry.MaxFrames + 1;

						}
					}

					if (szLine[0] == ';' || szLine[0] == '#') continue;
					if (szLine[0] == '\r') break;

					int iGroupID, iIndex, iXPos, iYPos, iFrameTime;
					// remove , 
					std::string strLine(szLine, strlen(szLine));
					std::replace(strLine.begin(), strLine.end(), ',', ' ');
					if (sscanf(strLine.c_str(), "%d", &iGroupID) == 1)
					{
						sscanf(strLine.c_str(), "%d %d %d %d %d", &iGroupID, &iIndex, &iXPos, &iYPos, &iFrameTime);
						airEntry.MaxFrames++;
						// create entry
						eAirData frame = { iGroupID, iIndex, iXPos, iYPos,iFrameTime };
						airEntry.vAnimData.push_back(frame);

					}


				}
				airEntry.AnimID = ID;
				vAnimations.push_back(airEntry);
				airEntry.vAnimData.clear();
				airEntry.MaxFrames = 0;
			}

		}

	}
	m_nAnimCount = i;
	fclose(pFile);
}

eAirEntry eAirReader::GetAnimation(int id)
{
	eAirEntry air;
	for (unsigned int i = 0; i < vAnimations.size(); i++)
	{
		if (vAnimations[i].AnimID == id) {
			air = vAnimations[i];
			break;
		}
		else
			air = vAnimations[0];
	}
	return air;
}


bool eAirReader::CheckName(std::string name)
{
	if (strName == name)
		return true;
	else
		return false;
}

eAirReader GetAIRFromName(std::string name)
{
	eAirReader temp;
	for (unsigned int i = 0; i < eAirManager.size(); i++)
	{
		if (eAirManager[i].CheckName(name)) {
			temp = eAirManager[i];
			break;
		}
		else
			temp = eAirManager[0];
	}
	return temp;
}

int GlobalGetAnimCount()
{
	int iCount = 0;
	for (unsigned int i = 0; i < eAirManager.size(); i++)
		iCount += eAirManager[i].m_nAnimCount;

	return iCount;
}
