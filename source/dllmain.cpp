// mugenhook
// ermaccer

#include "IniReader.h"
#include "MemoryMgr.h"

#include "code\eLog.h"
#include "code\eAirReader.h"
#include "code\eAnimatedPortraits.h"
#include "code\eCursorManager.h"
#include "code\eMugen.h"
#include "code\eSelectScreenManager.h"
#include "code\eInputManager.h"
#include "code\eSlidePorts.h"
#include "code\eSettingsManager.h"

using namespace Memory::VP;


void Init()
{
	SettingsMgr->Init();

	if (!SettingsMgr->bMugenhookFirstRun)
	{
		MessageBoxA(0, "MugenHook succesfully installed! \nPress OK to continue.", "MugenHook", 0);
		SettingsMgr->ini->WriteBoolean("Settings", "bFirstRun", true);
	}

	if (SettingsMgr->bUseLog) Log->Open("mugenhook_log.txt");


	if (SettingsMgr->bDevMode)
	{
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		if (SettingsMgr->bDev_RedirectMugenMessages)
			InjectHook(0x40C4A0, HookPushDebugMessage, PATCH_JUMP);
	}

	eMugenManager::Init();

	if (SettingsMgr->bEnableSlidePortraits)
	{
		eSlidePorts::ReadSettings();
		Patch<int>(0x412591 + 1, (int)eSlidePorts::HookMenuCase - ((int)0x412591 + 5));
		Patch<int>(0x4125B5 + 1, (int)eSlidePorts::HookSelectCase - ((int)0x4125B5 + 5));
	}

	if (SettingsMgr->iSelectableFighters)
		Patch<int>(0x4063F0, SettingsMgr->iSelectableFighters);

	if (SettingsMgr->bDisableNumerationInStageSelect)
	{
		Patch<char>(0x407E79, 0xE8);
		Nop(0x407E79 + 5, 1);
		Patch<char>(0x407E4F + 2, 8);
		Patch<int>(0x407E79 + 1, (int)eSelectScreenManager::HookStageNumeration - ((int)0x407E79 + 5));
	}

	if (SettingsMgr->bChangeStrings)
	{
		Patch<const char*>(0x40ADD8 + 3, SettingsMgr->ini->ReadString("Strings", "szGameModeSingle", "Single"));
		Patch<const char*>(0x40AE04 + 3, SettingsMgr->ini->ReadString("Strings", "szGameModeSimul", "Simul"));
		Patch<const char*>(0x40AE5F + 3, SettingsMgr->ini->ReadString("Strings", "szGameModeTurns", "Turns"));
		Patch<const char*>(0x40AE30 + 3, SettingsMgr->ini->ReadString("Strings", "szGameModeTag", "Tag"));
		Patch<const char*>(0x42BD3B + 7, SettingsMgr->ini->ReadString("Strings", "szKeyA", "A"));
		Patch<const char*>(0x42BD46 + 7, SettingsMgr->ini->ReadString("Strings", "szKeyB", "B"));
		Patch<const char*>(0x42BD51 + 7, SettingsMgr->ini->ReadString("Strings", "szKeyC", "C"));
		Patch<const char*>(0x42BD5C + 7, SettingsMgr->ini->ReadString("Strings", "szKeyX", "X"));
		Patch<const char*>(0x42BD67 + 7, SettingsMgr->ini->ReadString("Strings", "szKeyY", "Y"));
		Patch<const char*>(0x42BD72 + 7, SettingsMgr->ini->ReadString("Strings", "szKeyZ", "Z"));
		Patch<const char*>(0x42BD7D + 7, SettingsMgr->ini->ReadString("Strings", "szKeyStart", "Start"));
		Patch<const char*>(0x407E43 + 1, SettingsMgr->ini->ReadString("Strings", "szSelectStageRandom", "Stage: Random"));
		Patch<const char*>(0x407E73 + 1, SettingsMgr->ini->ReadString("Strings", "szSelectStage", "Stage %i: %s"));
		Patch<const char*>(0x41A591 + 1, SettingsMgr->ini->ReadString("Strings", "szAppTitle ", "M.U.G.E.N"));
	}

	if (SettingsMgr->bGameModeTagShow)    Patch<char>(0x40AE10, 6);
	if (SettingsMgr->bGameModeSimulHide)  Patch<char>(0x40ADE4, 4);
	if (SettingsMgr->bGameModeSingleHide) Patch<char>(0x40ADC2, 4);
	if (SettingsMgr->bGameModeTurnsHide)  Patch<char>(0x40AE3C, 4);

	Patch<int>(0x429B14 + 1, (int)HookDrawScreenMessage - ((int)0x429B14 + 5));

	if (SettingsMgr->bHookCursorTable)
		eCursorManager::ReadFile("cfg\\soundAnn.dat");

	if (SettingsMgr->bHookAnimatedPortraits)
	{
		eAnimatedPortraits::ReadFile("cfg\\animatedPortraits.dat");
		eAnimatedPortraits::ReadFramesFile("cfg\\frameLoader.dat");
		Patch<char>(0x404CE2, 0xE9);
		Patch<int>(0x404CE2 + 1, (int)eAnimatedPortraits::HookRequestSprites - ((int)0x404CE2 + 5));
		Log->PushMessage(false, "eAirReader::Info() | Loaded %d animations!\n", GlobalGetAnimCount());
	}

	Patch<int>(0x406FF3 + 1, (int)eAnimatedPortraits::HookDisplaySprites - ((int)0x406FF3 + 5));
	Patch<char>(0x406E51, 0xE9);
	Patch<int>(0x406E51 + 1, (int)eCursorManager::HookCursorFunction - ((int)0x406E51 + 5));

	PushDebugMessage("MugenHook loaded!\n");
}



extern "C"
{
	__declspec(dllexport) void InitializeASI()
	{
		if (*(char*)0x40B97B == 0 && *(char*)0x40B97D == 1 && *(char*)0x40B97F == 1)
			Init();
		else
			MessageBoxA(0, "Invalid M.U.G.E.N version! Only 1.1.0 supported!", "MugenHook", MB_ICONERROR);

	}
}

