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
	float     SpritesScaleY;
	float     SpritesScaleX;
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
	int  FindPortraitEntry(int row, int col);
	std::string GetCellFName(int row, int col);
	int  GetFrameTablePointer();


	// hooks
	void HookRequestSprites(int a1, int a2);
	int  HookDisplaySprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7);
	int  HookCharSpritePriority(int a1, int a2, int a3, int a4, int a5, float a6, float a7);
	void  HookCharSpritePrioritySecond(int a1, float a2, float a3);
}


