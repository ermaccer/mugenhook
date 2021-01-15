#include "eMagicBoxes.h"
#include "..\mugen\System.h"
#include "..\core\eLog.h"
#include "..\core\eSettingsManager.h"
#include "eSelectScreenManager.h"
#include "..\core\eCursor.h"
#include <iostream>

std::vector<int> eMagicBoxes::m_vMagicBoxes;
std::vector<eMugenCharacterInfo> eMagicBoxes::m_vMagicBoxesCharacters;
std::vector<eBoxEntry> eMagicBoxes::m_vMagicBoxesData;
int eMagicBoxes::m_nMagicBoxID;
bool eMagicBoxes::m_bScanRequired;

int eMagicBoxes::m_nMagicBoxTickCounter;


void eMagicBoxes::Init()
{
	m_nMagicBoxID = 0;
	m_bScanRequired = true;
	m_nMagicBoxTickCounter = 0;

	if (eSettingsManager::bHookMagicBoxes)
		ReadFile("cfg\\magicBoxes.dat");

}


void eMagicBoxes::ReadFile(const char * file)
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

			int iCharacter = 0;
			if (sscanf(szLine, "%d", &iCharacter) == 1)
			{
				int iTime = 0;

				sscanf(szLine, "%d %d", &iCharacter, &iTime);

				// create entry
				eBoxEntry entry = { iCharacter, iTime,0 };


				m_vMagicBoxesData.push_back(entry);

			}
		}
		eLog::PushMessage(__FUNCTION__, "Read %d entries\n", m_vMagicBoxesData.size());
		fclose(pFile);
	}
}

void eMagicBoxes::GetBox()
{
	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;

	if (m_bScanRequired)
	{
		for (int i = 0; i < eSystem::iColumns * eSystem::iRows; i++)
		{

			for (unsigned int a = 0; a < m_vMagicBoxesData.size(); a++)
				if (CharactersArray[i].ID == m_vMagicBoxesData[a].iCharacter)
				{
					m_vMagicBoxesCharacters.push_back(CharactersArray[i]);
					CharactersArray[i].ID = -1;
					CharactersArray[i].CharacterFlag |= CHAR_FLAG_HIDDEN;
				}


			if (CharactersArray[i].ID == -3)
			{
				m_nMagicBoxID = i;
				CharactersArray[m_nMagicBoxID].ID = -1;
				m_vMagicBoxes.push_back(m_nMagicBoxID);

			}
		}
		// hide required chars


	}

	m_bScanRequired = false;

}

void eMagicBoxes::Process()
{
	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;

	// TODO:: make a better timer solution
	for (unsigned int i = 0; i < m_vMagicBoxes.size(); i++)
	{
		if (eSystem::GetTimer() - m_nMagicBoxTickCounter <= m_vMagicBoxesData[i].iWaitTime)  return;
		m_vMagicBoxesData[i].iSwitch ^= 1;
	}


	for (unsigned int i = 0; i < m_vMagicBoxes.size(); i++)
	{
		//i = 1;

		if (m_vMagicBoxesData[i].iSwitch)
		{
			CharactersArray[m_vMagicBoxes[i]] = m_vMagicBoxesCharacters[i];
		}
		else
		{
			if (!(eCursor::Player1_Character == m_vMagicBoxes[i] || eCursor::Player2_Character == m_vMagicBoxes[i]))
			{
				CharactersArray[m_vMagicBoxes[i]].ID = -1;
				CharactersArray[m_vMagicBoxes[i]].SpritePointer = 0;
				sprintf(CharactersArray[m_vMagicBoxes[i]].Name, "%s", "");
			}

		}
		m_nMagicBoxTickCounter = eSystem::GetTimer();
	}





}

void eMagicBoxes::ResetBoxesStatus()
{

	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;

	for (unsigned int i = 0; i < m_vMagicBoxes.size(); i++)
	{
		m_vMagicBoxesData[i].iSwitch = 0;
		CharactersArray[m_vMagicBoxes[i]].ID = -1;
		CharactersArray[m_vMagicBoxes[i]].SpritePointer = 0;
		sprintf(CharactersArray[m_vMagicBoxes[i]].Name, "%s", "");
	}
	m_nMagicBoxTickCounter = eSystem::GetTimer();

}