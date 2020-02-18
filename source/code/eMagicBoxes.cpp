#include "eMagicBoxes.h"
#include "eCursorManager.h"
#include "eMugen.h"
#include "eLog.h"

#include <iostream>
#include <vector>


std::vector<int> vMagicBoxes;
std::vector<eMugenCharacter> vMagicBoxesCharacters;
std::vector<eBoxEntry> vMagicBoxesData;
int iMagicBoxID;
bool isScanRequired = true;

static int iTickCounter_p1 = *(int*)(*(int*)0x5040E8 + 0x11E98);

void eMagicBoxes::ReadFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		Log->PushMessage(false, "eMagicBoxes::ReadFile() | Failed! Tried to open: %s\n", file);
		Log->PushError();
	}


	if (pFile)
	{
		Log->PushMessage(false, "eMagicBoxes::ReadFile() | Success! Reading: %s\n", file);

		char szLine[2048];
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			// check if comment or empty line
			if (szLine[0] == ';' || szLine[0] == '#' || szLine[0] == '\n')
				continue;

			int iCharacter = 0;
			if (sscanf(szLine, "%d", &iCharacter) == 1)
			{
				int iTime;

				sscanf(szLine, "%d %d", &iCharacter, &iTime);

				// create entry
				eBoxEntry entry = { iCharacter, iTime,0 };
				

				vMagicBoxesData.push_back(entry);

			}
		}
		Log->PushMessage(false, "eMagicBoxes::ReadFile() | Read %d entries\n", vMagicBoxesData.size());
		fclose(pFile);
	}
}

void eMagicBoxes::GetBox()
{
	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

	if (isScanRequired)
	{
		for (int i = 0; i < MugenSystem->iColumns * MugenSystem->iRows; i++)
		{

		   for (int a = 0; a < vMagicBoxesData.size(); a++)
			   if (CharactersArray[i].ID == vMagicBoxesData[a].iCharacter)
			   {
				   vMagicBoxesCharacters.push_back(CharactersArray[i]);
				   CharactersArray[i].ID = -1;
				   CharactersArray[i].CharacterFlag |= CHAR_FLAG_HIDDEN;
			   }


			if (CharactersArray[i].ID == -3)
			{
				iMagicBoxID = i;
				CharactersArray[iMagicBoxID].ID = -1;
				vMagicBoxes.push_back(iMagicBoxID);

			}
		}
		// hide required chars


	}

	isScanRequired = false;

}

void eMagicBoxes::UpdateMagicBoxes()
{
	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

	// TODO:: make a better timer solution
	for (int i = 0; i < vMagicBoxes.size(); i++)
	{
		if (eMugenManager::GetTimer() - iTickCounter_p1 <= vMagicBoxesData[i].iWaitTime)  return;
		vMagicBoxesData[i].iSwitch ^= 1;
	}


	for (int i = 0; i < vMagicBoxes.size(); i++)
	{
		//i = 1;

		if (vMagicBoxesData[i].iSwitch )
		{

			CharactersArray[vMagicBoxes[i]] = vMagicBoxesCharacters[i];

		}
		else
		{
			if (!(TheCursor->Player1_Character == vMagicBoxes[i] || TheCursor->Player2_Character == vMagicBoxes[i]))
			{
				CharactersArray[vMagicBoxes[i]].ID = -1;
				CharactersArray[vMagicBoxes[i]].SpritePointer = 0;
				sprintf(CharactersArray[vMagicBoxes[i]].Name, "%s", "");
			}

		}
		iTickCounter_p1 = eMugenManager::GetTimer();
	}





}
