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
#include "code\eFightLogger.h"
#include "code\eMagicBoxes.h"
#include "code\eTagFix.h"
#include "code\eVariationsManager.h"

using namespace Memory::VP;

void NullFunc() {}

void Init()
{
	SettingsMgr->Init();

	InjectHook(0x406046, eInputManager::HookInputManager, PATCH_JUMP);

	if (!SettingsMgr->bMugenhookFirstRun)
	{
		MessageBoxA(0, "MugenHook succesfully installed! \nPress OK to continue.", "MugenHook", 0);
		SettingsMgr->ini->WriteBoolean("Settings", "bFirstRun", true);
	}

	if (SettingsMgr->bUseLog) Log->Open("mugenhook_log.txt");

	if (SettingsMgr->bUseFightLog)
	{
		eFightLogger::Init(SettingsMgr->szFightLogFile);
		InjectHook(0x412CCF, eFightLogger::HookLogMatchData, PATCH_CALL);
	}


	InjectHook(0x404029, eSelectScreenManager::HookSelectIDs, PATCH_JUMP);

	if (SettingsMgr->bDevMode)
	{
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		if (SettingsMgr->bDev_RedirectMugenMessages)
			InjectHook(0x40C4A0, HookPushDebugMessage, PATCH_JUMP);
	}

	if (SettingsMgr->bUseLeftRightInMenu)
	{
		Patch<char>(0x428E40 + 3, INPUT_ACTION_LEFT);
		Patch<char>(0x428E89 + 3, INPUT_ACTION_RIGHT);
	}

	if (SettingsMgr->bHookVariations)
	eVariationsManager::ReadFile("cfg\\variations.dat");

	eMugenManager::Init();

	if (SettingsMgr->bEnableTagFix)
	{
		Patch<int>(0x406DA0 + 8, (int)eTagFix::Hook);
		InjectHook(0x455961, eTagFix::HookGameModeCommand, PATCH_JUMP);
	}

	if (SettingsMgr->bHookMagicBoxes)
	eMagicBoxes::ReadFile("cfg\\magicBoxes.dat");

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
		Nop(0x407E79 + 5, 1);
		Patch<char>(0x407E4F + 2, 8);
		InjectHook(0x407E79, eSelectScreenManager::HookStageNumeration, PATCH_CALL);
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


	InjectHook(0x429B14, HookDrawScreenMessage, PATCH_CALL);

	// cursor placement
	InjectHook(0x406E51, eCursorManager::HookCursorFunction, PATCH_JUMP);



	if (SettingsMgr->bHookCursorTable)
		eCursorManager::ReadFile("cfg\\soundAnn.dat");

	if (SettingsMgr->bHookAnimatedPortraits)
	{
		eAnimatedPortraits::ReadFile("cfg\\animatedPortraits.dat");
		Log->PushMessage(false, "eAirReader::Info() | Loaded %d animations!\n", GlobalGetAnimCount());
	}



	// frame loader
	eAnimatedPortraits::ReadFramesFile("cfg\\frameLoader.dat");
	InjectHook(0x404CE2, eAnimatedPortraits::HookRequestSprites, PATCH_JUMP);

	// process select screen sprites
	InjectHook(0x406FF3, eAnimatedPortraits::HookDisplaySprites, PATCH_CALL);

	// version info
	InjectHook(0x429B14, HookDrawScreenMessage, PATCH_CALL);


	PushDebugMessage("MugenHook loaded!\n");
}


#ifndef _XP_SUPPORT
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
#endif

#ifdef _XP_SUPPORT
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (*(char*)0x40B97B == 0 && *(char*)0x40B97D == 1 && *(char*)0x40B97F == 1)
			Init();
		else
			MessageBoxA(0, "Invalid M.U.G.E.N version! Only 1.1.0 supported!", "MugenHook", MB_ICONERROR);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif

