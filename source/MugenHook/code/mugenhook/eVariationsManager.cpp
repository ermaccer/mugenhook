#include "eVariationsManager.h"
#include "..\core\eInputManager.h"
#include "..\core\eLog.h"
#include "..\mugen\System.h"
#include "eSelectScreenManager.h"
#include "..\core\eSettingsManager.h"
#include "..\core\eCursor.h"
#include "eAnimatedPortraits.h"
#include "..\mugen\Sound.h"

std::vector<eVariationCharacter> eVariationsManager::m_vVariations;

int eVariationsManager::m_nStartCounter;
int eVariationsManager::m_nStartCounterP2;
bool eVariationsManager::m_bVariationsScanRequired;
int eVariationsManager::m_tTickCounterVariations;



void eVariationsManager::Init()
{
	m_nStartCounter = 0;
	m_nStartCounterP2 = 0;
	m_bVariationsScanRequired = true;
	m_tTickCounterVariations = 0;

	if (eSettingsManager::bHookVariations)
		ReadFile("cfg\\variations.dat");
}

void eVariationsManager::ReadFile(const char * file)
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
		char* tempLine;
		int  errorCheck = 0;
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			if (szLine[0] == ';' || szLine[0] == '#' || szLine[0] == '\n')
				continue;
			tempLine = strtok(szLine, " ");
			int iRowID = 0;
			if (sscanf(szLine, "%d", &iRowID) == 1)
			{
				int iColumnID, iVariations;
				std::unique_ptr<int[]> aVariations;
				tempLine = strtok(NULL, " ");
				sscanf(tempLine, "%d", &iColumnID);
				tempLine = strtok(NULL, " ");
				sscanf(tempLine, "%d", &iVariations);

				aVariations = std::make_unique<int[]>(iVariations);

				for (int i = 0; i < iVariations; i++)
				{
					tempLine = strtok(NULL, " ");

					if (!tempLine)
					{
						eLog::PushMessage(__FUNCTION__, "Failed to read requested variation number! Read %d / %d (required).\n", errorCheck, iVariations);
						eLog::PushError();
						break;
					}
					errorCheck++;
					sscanf(tempLine, "%d", &aVariations[i]);
				}

				eVariationCharacter var;
				var.RowID = iRowID;
				var.ColumnID = iColumnID;
				var.iVariations = iVariations;
				for (int i = 0; i < iVariations; i++)
					var.vVariationIDs.push_back(aVariations[i]);

				m_vVariations.push_back(var);
			}
		}
		eLog::PushMessage(__FUNCTION__, "Read %d entries\n", m_vVariations.size());
		fclose(pFile);

	}
}

int eVariationsManager::FindVariationData(int row, int column)
{
	int iFind = -1;
	for (unsigned int i = 0; i < m_vVariations.size(); i++)
	{
		if (m_vVariations[i].RowID == row) {
			if (m_vVariations[i].ColumnID == column)
			{
				iFind = i;
				break;
			}
		}
	}
	return iFind;
}

void eVariationsManager::HideVariationCharacters()
{
	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;



	// store sprite pointers

	if (m_bVariationsScanRequired)
	{
		eMugenCharacterInfo emptyChar;
		emptyChar.ID = -1;
		emptyChar.SpritePointer = 0;

		for (unsigned int i = 0; i < m_vVariations.size(); i++)
		{
			for (int a = 0; a < eSystem::iColumns * eSystem::iRows; a++)
			{
				for (unsigned int b = 0; b < m_vVariations[i].vVariationIDs.size(); b++)
				{
					if (CharactersArray[a].ID == m_vVariations[i].vVariationIDs[b])
					{
						m_vVariations[i].vVariationChars.push_back(CharactersArray[a]);

						if (b == 0)
						{
							CharactersArray[a].CharacterFlag |= CHAR_FLAG_VARIATIONS;
							CharactersArray[a].CurrentVariation = 1;
							m_vVariations[i].iPlace = a;
						}


						if (b > 0 && eSettingsManager::bHideVariationCharacters)
						{
							CharactersArray[a].ID = -1;
							CharactersArray[a].CharacterFlag |= CHAR_FLAG_VARIATIONS | CHAR_FLAG_HIDDEN;
							//CharactersArray[a].SpritePointer = 0;
						}

					}
				}
			}
		}
		m_bVariationsScanRequired = false;
	}

}

void eVariationsManager::UpdateCharactersP1()
{

	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;

	// only update with available cursor
	if (eCursor::Player1_Character > -1 && !eCursor::Player1_Selected)
	{
		if (eInputManager::GetKeyAction(INPUT_ACTION_START))
		{
			if (eInputManager::CheckLastPlayer() == false)
				m_nStartCounter++;


			if (m_nStartCounter == 2)
			{

				int VariationID = FindVariationData(eCursor::Player1_Row, eCursor::Player1_Column);

				if (VariationID >= 0)
				{

					if (eSettingsManager::iVariationChangeGroup >= 0 && eSettingsManager::iVariationChangeIndex >= 0)
					{
						Sound* snd = eSystem::GetSystemSND();
						PlaySound(snd, eSettingsManager::iVariationChangeGroup, eSettingsManager::iVariationChangeIndex, 0, 3.390625f);
					}

					int CharID = m_vVariations[VariationID].iPlace;


					if (CharactersArray[CharID].CurrentVariation > m_vVariations[VariationID].iVariations - 1)
						CharactersArray[CharID].CurrentVariation = 0;

					char cVariations = CharactersArray[CharID].CurrentVariation;
					char cFlags = CharactersArray[CharID].CharacterFlag;
					char cExtraFlags = CharactersArray[CharID].ExtraFlags;

					CharactersArray[CharID] = m_vVariations[VariationID].vVariationChars[cVariations];
					CharactersArray[CharID].CurrentVariation = cVariations;
					CharactersArray[CharID].CharacterFlag |= cFlags;
					CharactersArray[CharID].ExtraFlags |= cExtraFlags;
					CharactersArray[CharID].CurrentIconIndex = 0;

					if (eInputManager::CheckLastPlayer() == false)
					{
						CharactersArray[CharID].CurrentVariation++;
					    eAnimatedPortraits::ResetFrameCounter(1);
					}

				}
				m_nStartCounter = 0;
			}

		}

	}
}

void eVariationsManager::UpdateCharactersP2()
{
	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;

	// only update with available cursor
	if (eCursor::Player2_Character > -1 && !eCursor::Player2_Selected)
	{
		if (eInputManager::GetKeyAction(INPUT_ACTION_START))
		{

			if (eInputManager::CheckLastPlayer() == true)
				m_nStartCounterP2++;
			if (m_nStartCounterP2 == 2)
			{

				int VariationID = FindVariationData(eCursor::Player2_Row, eCursor::Player2_Column);

				if (VariationID >= 0)
				{
					if (eSettingsManager::iVariationChangeGroup >= 0 && eSettingsManager::iVariationChangeIndex >= 0)
					{
						Sound* snd = eSystem::GetSystemSND();
						PlaySound(snd, eSettingsManager::iVariationChangeGroup, eSettingsManager::iVariationChangeIndex, 0, 3.390625f);
					}

					int CharID = m_vVariations[VariationID].iPlace;


					if (CharactersArray[CharID].CurrentVariation > m_vVariations[VariationID].iVariations - 1)
						CharactersArray[CharID].CurrentVariation = 0;

					char cVariations = CharactersArray[CharID].CurrentVariation;
					char cFlags = CharactersArray[CharID].CharacterFlag;
					char cExtraFlags = CharactersArray[CharID].ExtraFlags;
					CharactersArray[CharID] = m_vVariations[VariationID].vVariationChars[cVariations];
					CharactersArray[CharID].CurrentVariation = cVariations;
					CharactersArray[CharID].CharacterFlag |= cFlags;
					CharactersArray[CharID].ExtraFlags |= cExtraFlags;
					CharactersArray[CharID].CurrentIconIndex = 0;

					CharactersArray[CharID].CurrentVariation++;
					eAnimatedPortraits::ResetFrameCounter(2);
				}
				m_nStartCounterP2 = 0;
			}

		}

	}
}

void eVariationsManager::RestoreCharactersForGameplay()
{
}

void eVariationsManager::ProcessInactivity()
{
	// reset after not pressing
	if (eSystem::GetTimer() - m_tTickCounterVariations <= 60)  return;
	m_nStartCounter = 0; m_nStartCounterP2 = 0;
	m_tTickCounterVariations = eSystem::GetTimer();

}

void eVariationsManager::ResetVariationStatus()
{

	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;

	for (int i = 0; i < eSystem::iColumns * eSystem::iRows; i++)
	{
		if (CharactersArray[i].CharacterFlag & CHAR_FLAG_VARIATIONS)
		{
			CharactersArray[i].CurrentVariation = 1;
			CharactersArray[i] = m_vVariations[i].vVariationChars[0];
		}
			
	}
}

