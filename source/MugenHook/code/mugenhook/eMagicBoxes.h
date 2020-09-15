#pragma once
#include <vector>
#include "..\mugen\System.h"
struct eBoxEntry {
	int iCharacter;
	int iWaitTime;
	int iSwitch;
};

class eMagicBoxes {
public:
	static std::vector<int> m_vMagicBoxes;
	static std::vector<eMugenCharacter> m_vMagicBoxesCharacters;
	static std::vector<eBoxEntry> m_vMagicBoxesData;
	static int m_nMagicBoxID;
	static bool m_bScanRequired;

	static int m_nMagicBoxTickCounter;

	static void Init();
	static void ReadFile(const char* file);
	static void GetBox();
	static void Process();
	static void ResetBoxesStatus();
};	