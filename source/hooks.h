#pragma once
#include "MemoryMgr.h"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <fstream>
#include <filesystem>
#include "IniReader.h"
using namespace Memory::VP;

#define MAX_FRAMES 1000

struct eCursorEntry {
	int       id_row;
	int       id_column;
	int       snd_group;
	int       snd_select;
	int       snd_groupp2;
	int       snd_selectp2;
};

struct ePortraitEntry {
	int       id_row;
	int       id_column;
	int       group;
	int       group_p2;
	int       max_frames;
	int       frametime;
};

struct eSprite {
	int group;
	int index;
};

struct MugenCharacter {
	int id;
	int unknown;
	char name[48];
	char filename[512];
	char foldername[512];
	int  sprite_ptr;
	float scale;
};

struct MugenData {
	char pad[512];
	char gameFolder[512];
	char _pad[16];
	char lastStage[1024];
};

namespace eHooks {


	// anim ports
	static int iFrameCounter_p1 = 0;
	static int iFrameCounter_p2 = 0;

	static int iTickCounter_p1 = GetTickCount();
	static int iTickCounter_p2 = GetTickCount();

	// jumps
	static int cursor_eax;
	static int cursor_jmp = 0x406E56;
	static int sprite_jmp = 0x404CEF;
	static int FoundEntry;
	static int FoundEntryp2;
	static int Mugen_ResourcesPointer = 0x503388;
	static int Mugen_CharactersPointer = 0x503394;
	static int Mugen_DataPointer = 0x5040E8;


	static int row, column;

	// random stage sounds
	static int stage_group;
	static int stage_max;

	// default sounds
	static int select_default_grp;
	static int select_default_sound;
	static int select_default_grp2;
	static int select_default_sound2;

	// settings
	static bool bGameModeTagShow;
	static bool bGameModeSingleHide;
	static bool bGameModeSimulHide;
	static bool bGameModeTurnsHide;
	static bool bHookCursorTable;
	static bool bHookAnimatedPortraits;
	static bool bChangeStrings;
	static bool bRandomStageConfirmSounds;
	static int  iSelectableFighters;
	static bool bDumpCharacterInfo;

	static int lastEntry = 0;
	static int lastAnim = 0;
	static int lastFrame = 0;
	static int frameTablePtr = 0;
	static std::unique_ptr<eCursorEntry[]>       cursorTable; 
	static std::unique_ptr<ePortraitEntry[]>     animTable;
	static std::unique_ptr<eSprite[]>              frameTable;
	void  Init();
	void  HookCursorSounds();
	void  PrintCharacterNames();

	namespace CursorTabMan {
		void Init();
		void ReadFile(const char* file);
		void ProcessSelectScreen();
		int  FindSound(int row, int col);
		namespace AnimatedPortaits {
			void ReadFile(const char* file);
			void ReadFrameFile(const char* file);
			int LoadSprites(int a1, int a2);
			int FindFrame(int row, int col);
			void HookLoadSprites();
		}
	}
}