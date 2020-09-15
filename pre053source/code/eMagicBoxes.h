#pragma once
#include <vector>

struct eBoxEntry {
	int iCharacter;
	int iWaitTime;
	int iSwitch;
};

namespace eMagicBoxes {
	void ReadFile(const char* file);
	void GetBox();
	void UpdateMagicBoxes();
}