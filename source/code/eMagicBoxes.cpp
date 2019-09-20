#include "eMagicBoxes.h"
#include "eCursorManager.h"
#include "eMugen.h"

#include <iostream>

std::vector<eBoxEntry> BoxesTable;
int iTickCounter = eMugenManager::GetTimer();
int Player1_Character;
int GlobalI;
int pLinkSpritePointer;
static int pOgSpritePointer;

void eMagicBoxes::ReadFile(const char * file)
{

	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		printf("eMagicBoxes::ReadFile() | Failed! Tried to open: %s\n", file);
	}

	if (pFile)
	{
		printf("eMagicBoxes::ReadFile() | Success! Reading: %s\n", file);

		char szLine[2048];
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			// check if comment or empty line
			if (szLine[0] == ';' || szLine[0] == '#' || szLine[0] == '\n')
				continue;

			int iRow = 0;
			if (sscanf(szLine, "%d", &iRow) == 1)
			{
				int iColumn, iLinkChar, iOGChar, iTurnTime, iAccTime;
				sscanf(szLine, "%d %d %d %d %d %d", &iRow,&iColumn,&iLinkChar,&iOGChar,&iTurnTime,&iAccTime);

				// create entry
				eBoxEntry box = { iRow, iColumn, iLinkChar, iOGChar, iTurnTime, iAccTime };

				BoxesTable.push_back(box);
			}
		}
		printf("eMagicBoxes:::ReadFile() | Read %d entries\n", BoxesTable.size());
		fclose(pFile);
	}
}

int eMagicBoxes::FindBoxEntry(int row, int col)
{
	int iFind = 0;
	for (int i = 0; i < BoxesTable.size(); i++)
	{
		if (BoxesTable[i].Row == row) {
			if (BoxesTable[i].Column == col)
			{
				iFind = i;
				break;
			}
		}
	}
	return iFind;
}

int eMagicBoxes::FindCharacter(int id)
{
	int iFind = 0;
	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;
	for (int i = 0; i < eMugenManager::GetRows() * eMugenManager::GetColumns(); i++)
	{
		if (CharactersArray[i].ID == id) {
			iFind = i;
			break;
		}
	}
	return iFind;
}

void eMagicBoxes::UpdateBox()
{
	SwapToLink();

}

void eMagicBoxes::SwapToLink()
{
	bool bLocked, bOpen;
	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

	// close box
	if (eMugenManager::GetTimer() - iTickCounter <= BoxesTable[GlobalI].TurnTime)	return;
	printf("LNK: Swapping to %d from %d\n", BoxesTable[GlobalI].LinkCharacter, CharactersArray[Player1_Character].ID);
	CharactersArray[Player1_Character].ID = BoxesTable[GlobalI].LinkCharacter;
	pLinkSpritePointer = CharactersArray[FindCharacter(BoxesTable[GlobalI].LinkCharacter)].SpritePointer;
	CharactersArray[Player1_Character].SpritePointer = pLinkSpritePointer;
	iTickCounter = eMugenManager::GetTimer();
	SwapToOG();

}

void eMagicBoxes::SwapToOG()
{
	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

	if (eMugenManager::GetTimer() - iTickCounter <= BoxesTable[GlobalI].AccessTime) return;
	printf("Swapping to %d from %d\n", BoxesTable[GlobalI].OGCharacter, CharactersArray[Player1_Character].ID);
	CharactersArray[Player1_Character].ID = BoxesTable[GlobalI].OGCharacter;
	CharactersArray[Player1_Character].SpritePointer = CharactersArray[FindCharacter(BoxesTable[GlobalI].OGCharacter)].SpritePointer;
	iTickCounter = eMugenManager::GetTimer();
}

void eMagicBoxes::UpdateMagicBoxes()
{
	for (int i = 0; i < BoxesTable.size(); i++) {
		eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;
		Player1_Character = BoxesTable[i].Column + (BoxesTable[i].Row * eMugenManager::GetRows()) + (BoxesTable[i].Row * eMugenManager::GetRows() - BoxesTable[i].Row);
		UpdateBox();
		GlobalI = i;


	}
}
