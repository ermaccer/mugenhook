#pragma once
#include <string>
#include <vector>

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


class eAnimatedPortraits {
public:

	static std::vector<ePortraitEntry> AnimationTable;
	static std::vector<eFrameEntry> FrameTable;

	static int iFrameCounter_p1; 
	static int iFrameCounter_p2;

	static int iSelectCounter_p1;
	static int iSelectCounter_p2;

	static int iTickCounter_p1;
	static int iTickCounter_p2;

	static int pFrameTablePointer;
	static int pCurrentPlayerSprite;

	static bool bReadyToDrawSprite;

	static void Init();
	static void ReadFile(const char* file);
	static void ReadFramesFile(const char* file);

	static void Process();
	static void ProcessP2();

	static void ResetFrameCounter(int player);


	static int         FindPortraitEntry(int row, int col);
	static std::string GetCellFName(int row, int col);

	static void HookRequestSprites();
	static int  HookDisplaySprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7);
	static int  HookDisplaySprites2(int a1, int a2, int a3, int a4, int a5, float a6, float a7);
	static void HookGrabPlayerIDForDrawing();
};