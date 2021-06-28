#pragma once
#include <vector>
#include "..\mugen\Sound.h"

enum eCursorOperationTypes {
	MODE_ROW_COLUMN,
	MODE_CHAR_ID,
	MODE_CHAR_FILE
};


struct eCellEntry {
	int       RowID;
	int       ColumnID;
	int       SoundGroupID;
	int       SoundIndexID;
	int       SoundGroupP2ID;
	int       SoundIndexP2ID;
	int       CharID;
};


struct eSoundEntry {
	int			CharID;
	bool		IsCached;
	Sound*		SoundData;
};


class eCustomCursorManager {
public:
	static std::vector<eCellEntry> CellTable;
	static std::vector<eSoundEntry> SoundCellTable;
	static void Init();
	static void ReadFile(const char* file);
	static int  FindCell(int row, int col);
	static int  FindCellBasedOnID(int id);
	static int  FindSound(int id);
	static void HookSelectSoundPlayer();
	static void ProcessSoundP1();
	static void ProcessSoundP2();
	static void Process();
};