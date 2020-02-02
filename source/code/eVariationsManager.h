#pragma once
#include <memory>
#include <vector>
#include "eMugen.h"


struct eVariationCharacter {
	int RowID;
	int ColumnID;
	int iVariations;
	int iPlace;
	std::vector<int> vVariationIDs;
	std::vector<eMugenCharacter> vVariationChars;
};

namespace eVariationsManager {
	void  ReadFile(const char* file);
	int   FindVariationData(int row, int column);
	void  HideVariationCharacters();
	void  UpdateCharactersP1();
	void  UpdateCharactersP2();
	void  RestoreCharactersForGameplay();
	void  ProcessInactivity();
}