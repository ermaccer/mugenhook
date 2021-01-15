#pragma once
#include <memory>
#include <vector>
#include "..\mugen\System.h"


struct eVariationCharacter {
	int RowID;
	int ColumnID;
	int iVariations;
	int iPlace;
	std::vector<int> vVariationIDs;
	std::vector<eMugenCharacterInfo> vVariationChars;
};

class eVariationsManager {
public:

	static std::vector<eVariationCharacter> m_vVariations;

	static int m_nStartCounter;
	static int m_nStartCounterP2;
	static bool m_bVariationsScanRequired;
	static int m_tTickCounterVariations;


	static void Init();

	static void  ReadFile(const char* file);
	static int   FindVariationData(int row, int column);
	static void  HideVariationCharacters();
	static void  UpdateCharactersP1();
	static void  UpdateCharactersP2();
	static void  RestoreCharactersForGameplay();
	static void  ProcessInactivity();
	static void  ResetVariationStatus();
};