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


namespace eHooks {

	// jumps
	static int cursor_eax;
	static int cursor_jmp = 0x406E56;
	static int FoundEntry;
	static int FoundEntryp2;
	static int Mugen_ResourcesPointer = 0x503388;

	// settings
	static bool bGameModeTagShow;
	static bool bGameModeSingleHide;
	static bool bGameModeSimulHide;
	static bool bGameModeTurnsHide;
	static bool bHookCursorTable;
	static bool bChangeStrings;

	static int lastEntry = 0;
	static std::unique_ptr<eCursorEntry[]> cursorTable; 

	void Init();
	void  HookCursorSounds();
	namespace CursorTabMan {
		void Init();
		void ReadFile(const char* file);
		int  FindSound(int row, int col);
	}
}