#include "eCursorManager.h"
#include "eMugen.h"
#include "..\stdafx.h"
#include <vector>
#include <fstream>
#include <string>
std::vector<eCellEntry> CellTable;
int pCursorEax;
int pCursorJmp = 0x406E56;;
int Player1_Row, Player1_Column;
int Player2_Row, Player2_Column;
int Player1_Selected, Player2_Selected;
int Player1_Turns, Player2_Turns;
int Player_Training;
bool bEnablePos;
bool bEnableStageRandom;
bool bEnablePrint;
int  iRGRP, iRMAX;

void eCursorManager::ReadFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		printf("eCursorManager::ReadFile() | Failed! Tried to open: %s\n", file);
	}

	if (pFile)
	{
		printf("eCursorManager::ReadFile() | Success! Reading: %s\n", file);

		char szLine[2048];
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			// check if comment or empty line
			if (szLine[0] == ';' || szLine[0] == '#' || szLine[0] == '\n')
				continue;

			int iGroupID = 0;
			if (sscanf(szLine, "%d", &iGroupID) == 1)
			{
				int iIndex;
				int iSoundGRP;
				int iSoundGRP2;
				int iSoundIND;
				int iSoundIND2;
				sscanf(szLine, "%d %d %d %d %d %d", &iGroupID, &iIndex, &iSoundGRP, &iSoundIND, &iSoundGRP2, &iSoundIND2);

				// create entry
				eCellEntry cell = { iGroupID, iIndex, iSoundGRP, iSoundIND, iSoundGRP2, iSoundIND2 };

				CellTable.push_back(cell);
			}
		}
		printf("eCursorManager::ReadFile() | Read %d entries\n", CellTable.size());
		fclose(pFile);
	}
}

void eCursorManager::UpdateEax()
{
	Player1_Row = (*(int*)(pCursorEax + 14864 + 4));
	Player1_Column = (*(int*)(pCursorEax + 14864));
	Player1_Selected = (*(int*)(pCursorEax + 14592));
	Player1_Turns = (*(int*)(pCursorEax + 14592 + 36));
	Player_Training = (*(int*)(pCursorEax + 14864 + 24));

	Player2_Row = (*(int*)(pCursorEax + 14864 + 0xC0 + 4));
	Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0));
	Player2_Selected = (*(int*)(pCursorEax + 14592 + 4));
	Player2_Turns = (*(int*)(pCursorEax + 14592 + 36 + 116));
}

int eCursorManager::FindCell(int row, int col)
{
	int iFind = 0;
	for (int i = 0; i < CellTable.size(); i++)
	{
		if (CellTable[i].RowID == row) {
			if (CellTable[i].ColumnID == col)
			{
				iFind = i;
				break;
			}
		}
	}
	return iFind;
}



void __declspec(naked) eCursorManager::HookCursorFunction()
{
	_asm
	{
		mov pCursorEax, eax
		add eax, 14864
		pushad
	}
	eCursorManager::ProcessSelectScreen();
	_asm 
	{
		popad
		jmp pCursorJmp
	}
}


void eCursorManager::ProcessSelectScreen()
{
	UpdateEax();

	int Player1_Cell = FindCell(*(int*)(pCursorEax + 14864 + 16), *(int*)(pCursorEax + 14864));
	int Player2_Cell = FindCell(*(int*)(pCursorEax + 14864 + 0xC0 + 16), *(int*)(pCursorEax + 14864 + 0xC0));

	int Player1_Character = (Player1_Column + (Player1_Row * eMugenManager::GetRows()) + Player1_Row);
	int Player2_Character = (Player2_Column + (Player2_Row * eMugenManager::GetRows()) + Player2_Row);

	//printf("Character P1 %d ROW %d COL %d\n", Player1_Character, eCursorManager::GetPlayerRow(1),eCursorManager::GetPlayerColumn(1));

	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

	if (Player2_Turns == 0 && (CharactersArray[Player2_Character].ID >= -1)) {
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x344) = CellTable[Player2_Cell].SoundGroupP2ID;
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x34C) = CellTable[Player2_Cell].SoundIndexP2ID;
	}
	else {
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x344) = eMugenManager::GetSoundDoneGroup(2);
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x34C) = eMugenManager::GetSoundDoneIndex(2);
	}


	if (Player_Training == 1 && (CharactersArray[Player1_Character].ID >= -1)) {
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x340) = CellTable[Player1_Cell].SoundGroupID;
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x348) = CellTable[Player1_Cell].SoundIndexID;
	}
	else {
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x340) = eMugenManager::GetSoundDoneGroup(1);
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x348) = eMugenManager::GetSoundDoneIndex(1);
	}

    if (bEnableStageRandom)
	{
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x36C) = iRGRP;
		*(int*)(*(int*)eMugenManager::GetResourcesPointer() + 0x36C + 4) = rand() % iRMAX;
	}

	if (bEnablePos) printf("Player 1: Row: %d   Column: %d     \r", Player1_Row, Player1_Column);
	
	if (GetAsyncKeyState(VK_F5) && bEnablePrint) PrintCharacterNames();
	//printf("%d\n", eMugenManager::GetTimer());
}

void eCursorManager::EnablePositionDisplay()
{
	bEnablePos = true;
}

void eCursorManager::EnableRandomStages(int group, int max)
{
	bEnableStageRandom = true;
	iRGRP = group;
	iRMAX = max;
}

void eCursorManager::PrintCharacterNames()
{
	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;
	std::ofstream oFile("characters.txt", std::ofstream::binary);

	if (CharactersArray)
	{
		for (int i = 0; i < eMugenManager::GetRows() * eMugenManager::GetColumns(); i++)
		{
			std::string strName(CharactersArray[i].Name, strlen(CharactersArray[i].Name));
			oFile << "Character: " << strName << " ID:" << std::to_string(CharactersArray[i].ID) << "\n Folder: " << CharactersArray[i].FolderName << std::endl;
		}
		oFile.close();
	}
}

void eCursorManager::EnableDump()
{
	bEnablePrint = true;
}

int eCursorManager::GetPlayerRow(int player)
{
	if (player == 2)
		return Player2_Row;
	else
		return Player1_Row;
}

int eCursorManager::GetPlayerColumn(int player)
{
	if (player == 2)
		return Player2_Column;
	else
		return Player1_Column;
}

int eCursorManager::GetPlayerTraining()
{
	return Player_Training;
}

int eCursorManager::GetPlayerTurns(int player)
{
	if (player == 2)
		return Player2_Turns;
	else
		return Player1_Turns;
}

int eCursorManager::GetPlayerSelected(int player)
{
	if (player == 2)
		return Player2_Selected;
	else
		return Player1_Selected;
}

