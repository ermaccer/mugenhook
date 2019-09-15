#pragma once
#include "..\IniReader.h"
#include "..\MemoryMgr.h"

#include "eAirReader.h"
#include "eAnimatedPortraits.h"
#include "eCursorManager.h"
#include "eMugen.h"

using namespace Memory::VP;


void Init()
{
	bool bGameModeTagShow;
	bool bGameModeSingleHide;
	bool bGameModeSimulHide;
	bool bGameModeTurnsHide;
	bool bHookCursorTable;
	bool bHookAnimatedPortraits;
	bool bEnableSelectAnimations;
	bool bChangeStrings;
	bool bRandomStageConfirmSounds;
	bool bDumpCharacterInfo;
	bool bDevMode;
	bool bDev_DisplayPos;
	bool bDev_RedirectMugenMessages;

	int  iSelectableFighters;
	int  iRandomStageGroup;
	int  iRandomStageRandomMax;

	CIniReader ini("");
	bGameModeTagShow = ini.ReadBoolean("Settings", "bGameModeTagShow", 0);
	bGameModeSingleHide = ini.ReadBoolean("Settings", "bGameModeSingleHide", 0);
	bGameModeSimulHide = ini.ReadBoolean("Settings", "bGameModeSimulHide", 0);
	bGameModeTurnsHide = ini.ReadBoolean("Settings", "bGameModeTurnsHide", 0);
	bHookCursorTable = ini.ReadBoolean("Settings", "bHookCursorTable", 0);
	bHookAnimatedPortraits = ini.ReadBoolean("Settings", "bHookAnimatedPortraits", 0);
	bRandomStageConfirmSounds = ini.ReadBoolean("Settings", "bRandomStageConfirmSounds", 0);
	bEnableSelectAnimations = ini.ReadBoolean("Settings", "bEnableSelectAnimations", 0);
	bChangeStrings = ini.ReadBoolean("Settings", "bChangeStrings", 0);
	bDumpCharacterInfo = ini.ReadBoolean("Settings", "bDumpCharacterInfo", 0);
	bDevMode = ini.ReadBoolean("Settings", "bDevMode", 0);
	bDev_DisplayPos = ini.ReadBoolean("Settings", "bDev_DisplayPos", 0);
	bDev_RedirectMugenMessages = ini.ReadBoolean("Settings", "bDev_RedirectMugenMessages", 0);

	iRandomStageGroup = ini.ReadInteger("Settings", "iRandomStageGroup", 0);
	iRandomStageRandomMax = ini.ReadInteger("Settings", "iRandomStageRandomMax", 0);

	if (bDevMode)
	{
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		if (bDev_RedirectMugenMessages)
		   InjectHook(0x40C4A0, HookPushDebugMessage, PATCH_JUMP);
	}

	eMugenManager::Init();
	if (iSelectableFighters)
		Patch<int>(0x4063F0, iSelectableFighters);


	if (bChangeStrings)
	{
		Patch<const char*>(0x40ADD8 + 3, ini.ReadString("Strings", "szGameModeSingle", "Single"));
		Patch<const char*>(0x40AE04 + 3, ini.ReadString("Strings", "szGameModeSimul", "Simul"));
		Patch<const char*>(0x40AE5F + 3, ini.ReadString("Strings", "szGameModeTurns", "Turns"));
		Patch<const char*>(0x40AE30 + 3, ini.ReadString("Strings", "szGameModeTag", "Tag"));
		Patch<const char*>(0x42BD3B + 7, ini.ReadString("Strings", "szKeyA", "A"));
		Patch<const char*>(0x42BD46 + 7, ini.ReadString("Strings", "szKeyB", "B"));
		Patch<const char*>(0x42BD51 + 7, ini.ReadString("Strings", "szKeyC", "C"));
		Patch<const char*>(0x42BD5C + 7, ini.ReadString("Strings", "szKeyX", "X"));
		Patch<const char*>(0x42BD67 + 7, ini.ReadString("Strings", "szKeyY", "Y"));
		Patch<const char*>(0x42BD72 + 7, ini.ReadString("Strings", "szKeyZ", "Z"));
		Patch<const char*>(0x42BD7D + 7, ini.ReadString("Strings", "szKeyStart", "Start"));
		Patch<const char*>(0x407E43 + 1, ini.ReadString("Strings", "szSelectStageRandom", "Stage: Random"));
		Patch<const char*>(0x407E73 + 1, ini.ReadString("Strings", "szSelectStage", "Stage %i: %s"));
		Patch<const char*>(0x41A591 + 1, ini.ReadString("Strings", "szAppTitle ", "M.U.G.E.N"));
	}

	if (bGameModeTagShow)    Patch<char>(0x40AE10, 6);
	if (bGameModeSimulHide)  Patch<char>(0x40ADE4, 4);
	if (bGameModeSingleHide) Patch<char>(0x40ADC2, 4);
	if (bGameModeTurnsHide)  Patch<char>(0x40AE3C, 4);

	Patch<int>(0x429B14 + 1, (int)HookDrawScreenMessage - ((int)0x429B14 + 5));

	if (bHookCursorTable)
	{
		eCursorManager::ReadFile("cfg\\soundAnn.dat");
		Patch<char>(0x406E51, 0xE9);
		Patch<int>(0x406E51 + 1, (int)eCursorManager::HookCursorFunction - ((int)0x406E51 + 5));
		if (bDev_DisplayPos) eCursorManager::EnablePositionDisplay();
		if (bRandomStageConfirmSounds) eCursorManager::EnableRandomStages(iRandomStageGroup, iRandomStageRandomMax);
		if (bDumpCharacterInfo) eCursorManager::EnableDump();
		if (bHookAnimatedPortraits)
		{
			if (bEnableSelectAnimations) eAnimatedPortraits::EnableSelectAnimations();
			eAnimatedPortraits::ReadFile("cfg\\animatedPortraits.dat");
			eAnimatedPortraits::ReadFramesFile("cfg\\frameLoader.dat");
			// jump
			Patch<char>(0x404CE2, 0xE9);
			Patch<int>(0x404CE2 + 1, (int)eAnimatedPortraits::HookRequestSprites - ((int)0x404CE2 + 5));
			// animations
			Patch<int>(0x406FF3 + 1, (int)eAnimatedPortraits::HookDisplaySprites - ((int)0x406FF3 + 5)); 
			printf("eAirReader::Info() | Loaded %d animations!", GlobalGetAnimCount());

		}
	}
	
	PushDebugMessage("MugenHook loaded!\n");
	

}