#include "eVariationsManager.h"
#include "eLog.h"
#include "eMugen.h"
#include "eCursorManager.h"
#include "eInputManager.h"
#include "eSettingsManager.h"
#include <iostream>
#include <memory>
#include <vector>

std::vector<eVariationCharacter> vVariations;

int iStartCounter_p1 = 0;
int iStartCounter_p2 = 0;
bool bisScanRequired = true;
static int iTickCounter = *(int*)(*(int*)0x5040E8 + 0x11E98);

void eVariationsManager::ReadFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		Log->PushMessage(false, "eVariationsManager::ReadFile() | Failed! Tried to open: %s\n", file);
		Log->PushError();
	}

	if (pFile)
	{
		Log->PushMessage(false, "eVariationsManager::ReadFile() | Success! Reading: %s\n", file);

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
						Log->PushMessage(false, "eVariationsManager::ReadFile() | Failed to read requested variation number! Read %d / %d (required).\n", errorCheck, iVariations);
						Log->PushError();
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

				vVariations.push_back(var);
			}
		}
		Log->PushMessage(false, "eVariationsManager::ReadFile() | Read %d entries\n", vVariations.size());
		fclose(pFile);

	}
}

int eVariationsManager::FindVariationData(int row, int column)
{
	int iFind = -1;
	for (int i = 0; i < vVariations.size(); i++)
	{
		if (vVariations[i].RowID == row) {
			if (vVariations[i].ColumnID == column)
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
	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;



	// store sprite pointers

	if (bisScanRequired)
	{
		eMugenCharacter emptyChar;
		emptyChar.ID = -1;
		emptyChar.SpritePointer = 0;

		for (int i = 0; i < vVariations.size(); i++)
		{
			for (int a = 0; a < MugenSystem->iColumns * MugenSystem->iRows; a++)
			{
				for (int b = 0; b < vVariations[i].vVariationIDs.size(); b++)
				{
					if (CharactersArray[a].ID == vVariations[i].vVariationIDs[b])
					{
						vVariations[i].vVariationChars.push_back(CharactersArray[a]);
						
						if (b == 0)
						{
							CharactersArray[a].CharacterFlag |= CHAR_FLAG_VARIATIONS;
							CharactersArray[a].CurrentVariation = 1;
							vVariations[i].iPlace = a;
						}


						if (b > 0 && SettingsMgr->bHideVariationCharacters)
						{
							CharactersArray[a].ID = -1;
							CharactersArray[a].CharacterFlag |= CHAR_FLAG_VARIATIONS | CHAR_FLAG_HIDDEN;
							//CharactersArray[a].SpritePointer = 0;
						}

					}
				}
			}
		}
		bisScanRequired = false;
	}

}

void eVariationsManager::UpdateCharactersP1()
{

	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;


	// only update with available cursor
	if (TheCursor->Player1_Character > -1 && !TheCursor->Player1_Selected)
	{
		if (eInputManager::GetKeyAction(INPUT_ACTION_START))
		{
			if (eInputManager::CheckLastPlayer() == false)
			iStartCounter_p1++;


			if (iStartCounter_p1 == 2)
			{

				int VariationID = FindVariationData(TheCursor->Player1_Row, TheCursor->Player1_Column);

				if (VariationID >= 0)
				{
				
					int CharID = vVariations[VariationID].iPlace;


					if (CharactersArray[CharID].CurrentVariation > vVariations[VariationID].iVariations - 1)
						CharactersArray[CharID].CurrentVariation = 0;

					char cVariations = CharactersArray[CharID].CurrentVariation;
					CharactersArray[CharID] = vVariations[VariationID].vVariationChars[cVariations];
					CharactersArray[CharID].CurrentVariation = cVariations;
					if (eInputManager::CheckLastPlayer() == false)
					{
						CharactersArray[CharID].CurrentVariation++;

					}

				}
				iStartCounter_p1 = 0;
			}

		}

	}
}

void eVariationsManager::UpdateCharactersP2()
{
	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

	// only update with available cursor
	if (TheCursor->Player2_Character > -1 && !TheCursor->Player2_Selected)
	{
		if (eInputManager::GetKeyAction(INPUT_ACTION_START))
		{

			if (eInputManager::CheckLastPlayer() == true)
			iStartCounter_p2++;
			if (iStartCounter_p2 == 2)
			{

				int VariationID = FindVariationData(TheCursor->Player2_Row, TheCursor->Player2_Column);

				if (VariationID >= 0)
				{

					int CharID = vVariations[VariationID].iPlace;
					if (CharactersArray[CharID].CurrentVariation > vVariations[VariationID].iVariations - 1)
						CharactersArray[CharID].CurrentVariation = 0;

					char cVariations = CharactersArray[CharID].CurrentVariation;
					CharactersArray[CharID] = vVariations[VariationID].vVariationChars[cVariations];
					CharactersArray[CharID].CurrentVariation = cVariations;

					CharactersArray[CharID].CurrentVariation++;
				}
				iStartCounter_p2 = 0;
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
	if (eMugenManager::GetTimer() - iTickCounter <= 60)  return;
	iStartCounter_p1 = 0; iStartCounter_p2 = 0;
	iTickCounter = eMugenManager::GetTimer();

}
