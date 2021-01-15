#pragma once
#include <vector>

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



class eCustomCursorManager {
public:
	static std::vector<eCellEntry> CellTable;
	static void Init();
	static void ReadFile(const char* file);
	static int  FindCell(int row, int col);
	static int  FindCellBasedOnID(int id);

	static void Process();
};