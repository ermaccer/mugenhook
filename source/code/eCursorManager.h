#pragma once
struct eCellEntry {
	int       RowID;
	int       ColumnID;
	int       SoundGroupID;
	int       SoundIndexID;
	int       SoundGroupP2ID;
	int       SoundIndexP2ID;
};



namespace eCursorManager {
	void ReadFile(const char* file);
	void UpdateEax();
	int  FindCell(int row, int col);
	void HookCursorFunction();
	void ProcessSelectScreen();
	void EnablePositionDisplay();
	void EnableRandomStages(int group, int max);
	void PrintCharacterNames();
	void EnableDump();

	// manager stuff
	int  GetPlayerRow(int player);
	int  GetPlayerColumn(int player);
	int  GetPlayerTraining();
	int  GetPlayerTurns(int player);
	int  GetPlayerSelected(int player);
}