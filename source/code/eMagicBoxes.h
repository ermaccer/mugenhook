#pragma once
#include <vector>

struct eBoxEntry {
	int Row;
	int Column;
	int LinkCharacter;
	int OGCharacter;
	int TurnTime;
	int AccessTime;
};


namespace eMagicBoxes {
	void ReadFile(const char* file);
	int  FindBoxEntry(int row, int col);
	int  FindCharacter(int id);
	void UpdateBox();
	void SwapToLink();
	void SwapToOG();
	void UpdateMagicBoxes();
}