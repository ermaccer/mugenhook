#pragma once

struct eCellEntry {
	int       RowID;
	int       ColumnID;
	int       SoundGroupID;
	int       SoundIndexID;
	int       SoundGroupP2ID;
	int       SoundIndexP2ID;
};


class eCursor {
public:
	void Update();

	// vars
	int Player1_Row, Player1_Column;
	int Player2_Row, Player2_Column;
	int Player1_Selected, Player2_Selected;
	int Player1_Character, Player2_Character;
	int Player1_Turns, Player2_Turns;
	int Player_Training;
};

namespace eCursorManager {
	void ReadFile(const char* file);
	void Update();
	int  FindCell(int row, int col);
	void HookCursorFunction();
	void ProcessSelectScreen();
	void PrintCharacterNames();

	// fix
	void CursorWrapper();

	int  GetCursorEax();
}

extern eCursor* TheCursor;