#include "eAirReader.h"
#include <algorithm>
#include <string>
std::vector<eAirReader> eAirManager;

void eAirReader::Open(const char * name)
{
	strName = name;
	pFile = fopen(name, "rb");
	if (!pFile)
		printf("eAirReader::Open() | Failed! Cannot open %s!\n", name);

}

void eAirReader::ReadData()
{
		char szLine[1536];
		int i = 0;
		int ID = 0;
		eAirEntry airEntry;
		airEntry.MaxFrames = 0;
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			if (szLine[0] == '[')
			{
				i++;
				if (sscanf(szLine, "[Begin Action %d]", &ID) == 1)
				{
					while (fgets(szLine, sizeof(szLine), pFile))
					{
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
		iAnimCount = i;
		fclose(pFile);
}

eAirEntry eAirReader::GetAnimation(int id)
{
	eAirEntry air;
	for (int i = 0; i < vAnimations.size(); i++)
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

int eAirReader::GetAnimCount()
{
	return iAnimCount;
}

void eAirReader::PrintAnimID()
{
	printf("Loaded anims:\n");
	for (int i = 0; i < vAnimations.size(); i++)
	{
		printf("%d ", vAnimations[i].AnimID);
		for (int a = 0; a < vAnimations[i].vAnimData.size(); a++)
			printf("-%d-", vAnimations[i].vAnimData[a].Index);
	}
	printf("\n");
}

bool eAirReader::CheckName(std::string name)
{
	if (strName == name)
		return true;
	else
		return false;
}

eAirEntry GlobalGetAnimation(int id)
{
	return eAirEntry();
}

eAirReader GlobalGetAirEntryFromName(std::string name)
{
	eAirReader temp;
	for (int i = 0; i < eAirManager.size(); i++)
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
	for (int i = 0; i < eAirManager.size(); i++)
	{
		iCount += eAirManager[i].GetAnimCount();
	}
	return iCount;
}
