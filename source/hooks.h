#pragma once
#include "MemoryMgr.h"
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>
#include "IniReader.h"
using namespace Memory::VP;


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
	int       max_frames;
	int       frametime;
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

	// jumps
	static int cursor_eax;
	static int cursor_jmp = 0x406E56;
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
	static bool bChangeStrings;
	static bool bRandomStageConfirmSounds;
	static int  iSelectableFighters;
	static bool bDumpCharacterInfo;

	static int lastEntry = 0;
	static int lastAnim = 0;
	static std::unique_ptr<eCursorEntry[]>       cursorTable; 
	static std::unique_ptr<ePortraitEntry[]>     animTable;
	void  Init();
	void  HookCursorSounds();
	void  PrintCharacterNames();
	

	namespace CursorTabMan {
		void Init();
		void ReadFile(const char* file);
		void ProcessSelectScreen();
		namespace AnimatedPortaits {
			void ReadFile(const char* file);
			int DisplaySprites(int a1, int a2, int a3, int a4, int a5, float x, float y);
			int LoadSprites(int a1, int a2);
		}
		int  FindSound(int row, int col);
	}
}