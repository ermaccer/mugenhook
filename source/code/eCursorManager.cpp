#include "eSettingsManager.h"
#include "eCursorManager.h"
#include "eMugen.h"
#include "eSelectScreenManager.h"
#include "eLog.h"
#include "..\stdafx.h"
#include <vector>
#include <fstream>
#include <string>
std::vector<eCellEntry> CellTable;
int pCursorEax;
int pCursorJmp = 0x406E56;;


eCursor* TheCursor = new eCursor();

void eCursorManager::ReadFile(const char * file)
{
	FILE* pFile = fopen(file, "rb");

	if (!pFile)
	{
		Log->PushMessage(false,"eCursorManager::ReadFile() | Failed! Tried to open: %s\n", file);
		Log->PushError();
	}

	if (pFile)
	{
		Log->PushMessage(false,"eCursorManager::ReadFile() | Success! Reading: %s\n", file);

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
		Log->PushMessage(false,"eCursorManager::ReadFile() | Read %d entries\n", CellTable.size());
		fclose(pFile);
	}
}

void eCursorManager::Update()
{

	if (SettingsMgr->bDev_DisplayPos) printf("Player 1: Row: %d   Column: %d     \r", TheCursor->Player1_Row, TheCursor->Player1_Column);

	if (GetAsyncKeyState(VK_F5) && SettingsMgr->bDumpCharacterInfo) PrintCharacterNames();

	if (SettingsMgr->bRandomStageConfirmSounds)
	{
		*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x36C) = SettingsMgr->iRandomStageGroup;
		*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x36C + 4) = rand() % SettingsMgr->iRandomStageRandomMax;
	}

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
	TheCursor->Update();
	Update();
	if (SettingsMgr->bHookCursorTable)  eCursorManager::ProcessSelectScreen();
	_asm 
	{
		popad
		jmp pCursorJmp
	}
}

void eCursorManager::ProcessSelectScreen()
{
	Update();

	int Player1_Cell = FindCell(TheCursor->Player1_Row, TheCursor->Player1_Column);
	int Player2_Cell = FindCell(TheCursor->Player2_Row, TheCursor->Player2_Column);

	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;

	// p2 cursor is used for training selection
	if (eMugenManager::GetGameplayMode() == MODE_TRAINING)
	{
		if (TheCursor->Player1_Selected)
		{
			if (CharactersArray[TheCursor->Player2_Character].ID == -2) {
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x340) = MugenSystem->iSoundP2DoneGroup;
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x348) = MugenSystem->iSoundP2DoneIndex;
			}
			else {
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x340) = CellTable[Player2_Cell].SoundGroupP2ID;
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x348) = CellTable[Player2_Cell].SoundIndexP2ID;
			}
		}
		else
		{
			if (CharactersArray[TheCursor->Player1_Character].ID == -2) {
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x340) = MugenSystem->iSoundP1DoneGroup;
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x348) = MugenSystem->iSoundP1DoneIndex;
			}
			else {
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x340) = CellTable[Player1_Cell].SoundGroupID;
				*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x348) = CellTable[Player1_Cell].SoundIndexID;

			}
		}

	}
	else
	{
		if (CharactersArray[TheCursor->Player2_Character].ID == -2) {
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x344) = MugenSystem->iSoundP2DoneGroup;
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x34C) = MugenSystem->iSoundP2DoneIndex;
		}
		else {
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x344) = CellTable[Player2_Cell].SoundGroupP2ID;
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x34C) = CellTable[Player2_Cell].SoundIndexP2ID;
		}

		// watch mode todo
		if (CharactersArray[TheCursor->Player1_Character].ID == -2 ||  TheCursor->Player1_Selected && eMugenManager::GetGameplayMode() == MODE_WATCH) {
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x340) = MugenSystem->iSoundP1DoneGroup;
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x348) = MugenSystem->iSoundP1DoneIndex;
		}
		else {
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x340) = CellTable[Player1_Cell].SoundGroupID;
			*(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x348) = CellTable[Player1_Cell].SoundIndexID;

		}
	}


}


void eCursorManager::PrintCharacterNames()
{
	eMugenCharacter* CharactersArray = *(eMugenCharacter**)0x503394;
	std::ofstream oFile("characters.txt", std::ofstream::binary);

	if (CharactersArray)
	{
		for (int i = 0; i < MugenSystem->iRows * MugenSystem->iColumns; i++)
		{
			std::string strName(CharactersArray[i].Name, strlen(CharactersArray[i].Name));
			oFile << "Character: " << strName << " ID:" << std::to_string(CharactersArray[i].ID) << "\n Folder: " << CharactersArray[i].FolderName << std::endl;
		}
		oFile.close();
	}
}


int eCursorManager::GetCursorEax()
{
	return pCursorEax;
}

void eCursor::Update()
{
	// update multi cursor
	Player1_Character = *(int*)(pCursorEax + 14846 + 18 + 4 + 4);
	Player1_Row = (*(int*)(pCursorEax + 14846 + 18 + 4));
	Player1_Column = (*(int*)(pCursorEax + 14864));

	if (*(int*)(pCursorEax + 14846 - 214) == 2) {
		if (*(int*)((pCursorEax + 14846 + 42)) == 2) {
			Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4));
			Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4 + 4));
			Player1_Column = (*(int*)(pCursorEax + 14864 + 48));
		}
	}
	else if (*(int*)(pCursorEax + 14846 - 214) == 3) {
		if (*(int*)((pCursorEax + 14846 + 42)) == 2) {
			Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4));
			Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4 + 4));
			Player1_Column = (*(int*)(pCursorEax + 14864 + 48));
			if (*(int*)((pCursorEax + 14846 + 48 + 42)) == 2) {
				Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4));
				Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4 + 4));
				Player1_Column = (*(int*)(pCursorEax + 14864 + 48 + 48));
			}
		}
	}
	else if (*(int*)(pCursorEax + 14846 - 214) == 4) {
		if (*(int*)((pCursorEax + 14846 + 42)) == 2) {
			Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4));
			Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 18 + 4 + 4));
			Player1_Column = (*(int*)(pCursorEax + 14864 + 48));
			if (*(int*)((pCursorEax + 14846 + 48 + 42)) == 2) {
				Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4));
				Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 48 + 18 + 4 + 4));
				Player1_Column = (*(int*)(pCursorEax + 14864 + 48 + 48));
				if (*(int*)((pCursorEax + 14846 + 48 + 48 + 42)) == 2) {
					Player1_Row = (*(int*)(pCursorEax + 14846 + 48 + 48 + 48 + 18 + 4));
					Player1_Character = (*(int*)(pCursorEax + 14846 + 48 + 48 + 48 + 18 + 4 + 4));
					Player1_Column = (*(int*)(pCursorEax + 14864 + 48 + 48 + 48));
				}
			}

		}
	}


	Player1_Selected = (*(int*)(pCursorEax + 14592));
	Player1_Turns = (*(int*)(pCursorEax + 14592 + 36));
	Player_Training = (*(int*)(pCursorEax + 14864 + 24));

	Player2_Selected = (*(int*)(pCursorEax + 14592 + 4));
	Player2_Turns = (*(int*)(pCursorEax + 14592 + 36 + 116));


	Player2_Row = (*(int*)(pCursorEax + 14864 + 0xC0 + 4));
	Player2_Character = *(int*)(pCursorEax + 14864 + 0xC0 + 4 + 4);
	Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0));

	if (*(int*)(pCursorEax + 14846 - 98) == 2) {
		if (*(int*)((pCursorEax + 14846 + 0xC0 + 42)) == 2) {
			Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4));
			Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4 + 4));
			Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48));
		}
	}
	else if (*(int*)(pCursorEax + 14846 - 98) == 3) {
		if (*(int*)((pCursorEax + 14846 + 0xC0 + 42)) == 2) {
			Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4));
			Player2_Character  = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4 + 4));
			Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48));
			if (*(int*)((pCursorEax + 14846 + 0xC0 + 48 + 42)) == 2) {
				Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 18 + 4));
				Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 18 + 4 + 4));
				Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48 + 48));
			}
		}
	}
	else if (*(int*)(pCursorEax + 14846 - 98) == 4) {
		if (*(int*)((pCursorEax + 14846 + 0xC0 + 42)) == 2) {
			Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4));
			Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 18 + 4 + 4));
			Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48));
			if (*(int*)((pCursorEax + 14846 + 0xC0 + 48 + 42)) == 2) {
				Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 18 + 4));
				Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 18 + 4 + 4));
				Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48 + 48));
				if (*(int*)((pCursorEax + 14846 + 0xC0 + 48 + 48 + 42)) == 2) {
					Player2_Row = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 48 + 18 + 4));
					Player2_Character = (*(int*)(pCursorEax + 14846 + 0xC0 + 48 + 48 + 48 + 18 + 4 + 4));
					Player2_Column = (*(int*)(pCursorEax + 14864 + 0xC0 + 48 + 48 + 48));
				}
			}
		}

	}
}
