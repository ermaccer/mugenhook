#include "eCustomCursorManager.h"
#include "..\core\eLog.h"
#include "..\mugen\System.h"
#include "..\core\eCursor.h"
#include "eMugenConfig.h"
#include "..\mugen\Sound.h"
#include "..\core\eSettingsManager.h"
#include "..\core\eInputManager.h"

std::vector<eCellEntry> eCustomCursorManager::CellTable;
std::vector<eSoundEntry> eCustomCursorManager::SoundCellTable;
void eCustomCursorManager::Init()
{
	CellTable.clear();
	SoundCellTable.clear();
	if (eSettingsManager::iCursorTableOperationType == MODE_CHAR_FILE)
		InjectHook(0x40686A, HookSelectSoundPlayer, PATCH_CALL);
}

void eCustomCursorManager::ReadFile(const char * file)
{
	if (eSettingsManager::iCursorTableOperationType == MODE_CHAR_FILE)
		return;

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

			int iGroupID = 0;
			if (sscanf(szLine, "%d", &iGroupID) == 1)
			{
				int iIndex = 0;
				int iSoundGRP = 0;
				int iSoundGRP2 = 0;
				int iSoundIND = 0;
				int iSoundIND2 = 0;
				int iCharacterID = 0;

				if (eSettingsManager::iCursorTableOperationType == MODE_CHAR_ID)
					sscanf(szLine, "%d %d %d %d %d", &iCharacterID, &iSoundGRP, &iSoundIND, &iSoundGRP2, &iSoundIND2);
				else
				    sscanf(szLine, "%d %d %d %d %d %d", &iGroupID, &iIndex, &iSoundGRP, &iSoundIND, &iSoundGRP2, &iSoundIND2);

				// create entry
				eCellEntry cell = { iGroupID, iIndex, iSoundGRP, iSoundIND, iSoundGRP2, iSoundIND2, iCharacterID };
				CellTable.push_back(cell);

			}
		}
		eLog::PushMessage(__FUNCTION__, "Read %d entries\n", CellTable.size());
		fclose(pFile);
	}
}

int eCustomCursorManager::FindCell(int row, int col)
{
	int iFind = 0;
	for (unsigned int i = 0; i < CellTable.size(); i++)
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

int eCustomCursorManager::FindCellBasedOnID(int id)
{
	int iFind = 0;
	for (unsigned int i = 0; i < CellTable.size(); i++)
	{
		if (CellTable[i].CharID == id) {
			iFind = i;
			break;
		}
	}
	return iFind;
}

void eCustomCursorManager::HookSelectSoundPlayer()
{
	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;
	int player = eInputManager::CheckLastPlayer();

	if (!player)
	{
		if (eSystem::GetGameplayMode() == MODE_TRAINING)
		{
			if (eCursor::Player1_Selected)
			{
				if (CharactersArray[eCursor::Player2_Character].ID == -2) {
					PlaySound(eSystem::GetSystemSND(), eSystem::iSoundP2DoneGroup, eSystem::iSoundP2DoneIndex, 5, 100.0f);
				}
				else {
					PlaySound(eSystem::GetSystemSND(), eSystem::iSoundP2DoneGroup, eSystem::iSoundP2DoneIndex, 5, 100.0f);
					while (!SoundCellTable[eCursor::Player2_Character].IsCached)
						Sleep(1);
					PlaySound(SoundCellTable[eCursor::Player2_Character].SoundData, eSettingsManager::iCursorDefaultGroup, eSettingsManager::iCursorDefaultIndex, 10, 3.390625f);
				}
			}
			else
			{
				if (CharactersArray[eCursor::Player1_Character].ID == -2) {
					PlaySound(eSystem::GetSystemSND(), eSystem::iSoundP1DoneGroup, eSystem::iSoundP1DoneIndex, 5, 100.0f);
				}
				else {
					PlaySound(eSystem::GetSystemSND(), eSystem::iSoundP1DoneGroup, eSystem::iSoundP1DoneIndex, 5, 100.0f);
					while (!SoundCellTable[eCursor::Player1_Character].IsCached)
						Sleep(1);
					PlaySound(SoundCellTable[eCursor::Player1_Character].SoundData, eSettingsManager::iCursorDefaultGroup, eSettingsManager::iCursorDefaultIndex, 10, 3.390625f);
				}
			}

		}
		else
		{
			if (!(CharactersArray[eCursor::Player1_Character].ID == -2))
			{
				PlaySound(eSystem::GetSystemSND(), eSystem::iSoundP1DoneGroup, eSystem::iSoundP1DoneIndex, 5, 100.0f);
				while (!SoundCellTable[eCursor::Player1_Character].IsCached)
					Sleep(1);

				PlaySound(SoundCellTable[eCursor::Player1_Character].SoundData, eSettingsManager::iCursorDefaultGroup, eSettingsManager::iCursorDefaultIndex, 10, 3.390625f);
			}

			if (CharactersArray[eCursor::Player1_Character].ID == -2 || eCursor::Player1_Selected && eSystem::GetGameplayMode() == MODE_WATCH) {
				PlaySound(eSystem::GetSystemSND(), eSystem::iSoundP1DoneGroup, eSystem::iSoundP1DoneIndex, 5, 100.0f);
			}
			else {
				while (!SoundCellTable[eCursor::Player1_Character].IsCached)
					Sleep(1);
				PlaySound(SoundCellTable[eCursor::Player1_Character].SoundData, eSettingsManager::iCursorDefaultGroup, eSettingsManager::iCursorDefaultIndex, 10, 3.390625f);

			}
		}

	}
	else
	{
		if (!(CharactersArray[eCursor::Player2_Character].ID == -2))
		{
			PlaySound(eSystem::GetSystemSND(), eSystem::iSoundP2DoneGroup, eSystem::iSoundP2DoneIndex, 5, 100.0f);

			while (!SoundCellTable[eCursor::Player2_Character].IsCached)
				Sleep(1);

			PlaySound(SoundCellTable[eCursor::Player2_Character].SoundData, 6, 1, 10, 3.390625f);
		}
		else
		{
			PlaySound(eSystem::GetSystemSND(), eSystem::iSoundP2DoneGroup, eSystem::iSoundP2DoneIndex, 5, 100.0f);
		}

	}

}

void eCustomCursorManager::Process()
{
	if (eSettingsManager::iCursorTableOperationType == MODE_CHAR_FILE)
		return;

	eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;
	int Player1_Cell = 0, Player2_Cell = 0;

	if (CellTable.size() > 0)
	{
		switch (eSettingsManager::iCursorTableOperationType)
		{
		case MODE_CHAR_ID:
			Player1_Cell = FindCellBasedOnID(CharactersArray[eCursor::Player1_Character].ID);
			Player2_Cell = FindCellBasedOnID(CharactersArray[eCursor::Player2_Character].ID);
			break;
		default:
			Player1_Cell = FindCell(eCursor::Player1_Row, eCursor::Player1_Column);
			Player2_Cell = FindCell(eCursor::Player2_Row, eCursor::Player2_Column);
			break;
		}




		// p2 cursor is used for training selection
		if (eSystem::GetGameplayMode() == MODE_TRAINING)
		{
			if (eCursor::Player1_Selected)
			{
				if (CharactersArray[eCursor::Player2_Character].ID == -2) {
					eMugenConfig::SetCursorSound(1, eSystem::iSoundP2DoneGroup, eSystem::iSoundP2DoneIndex);
				}
				else {
					eMugenConfig::SetCursorSound(1, CellTable[Player2_Cell].SoundGroupP2ID, CellTable[Player2_Cell].SoundIndexP2ID);
				}
			}
			else
			{
				if (CharactersArray[eCursor::Player1_Character].ID == -2) {
					eMugenConfig::SetCursorSound(1, eSystem::iSoundP1DoneGroup, eSystem::iSoundP1DoneIndex);
				}
				else {
					eMugenConfig::SetCursorSound(1, CellTable[Player1_Cell].SoundGroupID, CellTable[Player1_Cell].SoundIndexID);
				}
			}

		}
		else
		{
			if (CharactersArray[eCursor::Player2_Character].ID == -2) {
				eMugenConfig::SetCursorSound(2, eSystem::iSoundP2DoneGroup, eSystem::iSoundP2DoneIndex);
			}
			else {
				eMugenConfig::SetCursorSound(2, CellTable[Player2_Cell].SoundGroupP2ID, CellTable[Player2_Cell].SoundIndexP2ID);
			}

			if (CharactersArray[eCursor::Player1_Character].ID == -2 || eCursor::Player1_Selected && eSystem::GetGameplayMode() == MODE_WATCH) {
				eMugenConfig::SetCursorSound(1, eSystem::iSoundP1DoneGroup, eSystem::iSoundP1DoneIndex);
			}
			else {
				eMugenConfig::SetCursorSound(1, CellTable[Player1_Cell].SoundGroupID, CellTable[Player1_Cell].SoundIndexID);

			}
		}
	}
}

