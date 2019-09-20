#pragma once
#include <string>


struct ePortraitEntry {
	int       RowID;
	int       ColumnID;
	int       SelectAnimationID;
	int       SelectAnimationAlternateID;
	int       WinAnimationID;
	int       WinAnimationAlternateID;
	std::string AirFileName;
};


struct eFrameEntry {
	int       SpriteGroup;
	int       SpriteIndex;
};

namespace eAnimatedPortraits {
	void Init();
	void ReadFile(const char* file);
	void ReadFramesFile(const char* file);
	void ProcessSelectScreen();
	void ProcessSelectScreenP2();
	void EnableDebug();
	void EnableSelectAnimations();
	void EnableAltAnims();
	int  FindPortraitEntry(int row, int col);
	std::string GetCellFName(int row, int col);
	int  GetFrameTablePointer();

	// hooks
	void HookRequestSprites(int a1, int a2);
	int  HookDisplaySprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7);
}
