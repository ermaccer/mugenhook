// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "code\mugenhook\base.h"
#include "code/core/eSettingsManager.h"
#include "code/core/eCursor.h"
#include "code/mugen/System.h"
#include "code/mugenhook/eCustomCursorManager.h"
#include "code/mugenhook/eAnimatedPortraits.h"
#include "code/mugenhook/eSelectScreenManager.h"
#include "code/mugenhook/eCommonHooks.h"
#include "code/mugenhook/eMenuManager.h"
#include "code/mugenhook/eSlidingPortraits.h"
#include "code/core/eInputManager.h"
#include "code/mugen/Common.h"
#include "code/mugenhook/eMagicBoxes.h"
#include "code/core/eLog.h"
#include "code/mugenhook/eVariationsManager.h"
#include "code/mugenhook/eAnimatedIcons.h"
#include "code/mugenhook/eTagFix.h"
#include "code/mugenhook/eStageAnnouncer.h"


void Init()
{
	eSettingsManager::Init();
	eInputManager::Init();
	eMenuManager::Init();

	if (!eSettingsManager::bMugenhookFirstRun)
	{
		MessageBox(0, "MugenHook succesfully installed!\nPress OK to continue.", "MugenHook", 0);
		eSettingsManager::ini->WriteBoolean("Settings", "bFirstRun", true);
	}


	if (eSettingsManager::bUseLog) eLog::Open("mugenhook_log.txt");


	if (eSettingsManager::bDevMode)
	{
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		if (eSettingsManager::bDev_RedirectMugenMessages)
			InjectHook(0x40C4A0, eCommonHooks::HookPushDebugMessage, PATCH_JUMP);
	}



	eSystem::Init();
	eCursor::Init();
	eCommonHooks::Init();
	eCustomCursorManager::Init();
	if (eSettingsManager::bHookCursorTable)
		eCustomCursorManager::ReadFile("cfg\\soundAnn.dat");
	eSelectScreenManager::Init();
	eAnimatedPortraits::Init();
	eSlidingPortraits::Init();
	eMagicBoxes::Init();
	eVariationsManager::Init();
	eAnimatedIcons::Init();
	eTagFix::Init();

	if (eSettingsManager::bUseLeftRightInMenu)
	{
		Patch<char>(0x428E40 + 3, INPUT_ACTION_LEFT);
		Patch<char>(0x428E89 + 3, INPUT_ACTION_RIGHT);
	}


	if (eSettingsManager::iSelectableFighters)
		Patch<int>(0x4063F0, eSettingsManager::iSelectableFighters);

	if (eSettingsManager::bDisableNumerationInStageSelect)
	{
		Nop(0x407E79 + 5, 1);
		Patch<char>(0x407E4F + 2, 8);
		InjectHook(0x407E79, eSelectScreenManager::HookStageNumeration, PATCH_CALL);
	}


	if (eSettingsManager::bChangeStrings)
	{
		Patch<const char*>(0x40ADD8 + 3, eSettingsManager::ini->ReadString("Strings", "szGameModeSingle", "Single"));
		Patch<const char*>(0x40AE04 + 3, eSettingsManager::ini->ReadString("Strings", "szGameModeSimul", "Simul"));
		Patch<const char*>(0x40AE5F + 3, eSettingsManager::ini->ReadString("Strings", "szGameModeTurns", "Turns"));
		Patch<const char*>(0x40AE30 + 3, eSettingsManager::ini->ReadString("Strings", "szGameModeTag", "Tag"));
		Patch<const char*>(0x42BD3B + 7, eSettingsManager::ini->ReadString("Strings", "szKeyA", "A"));
		Patch<const char*>(0x42BD46 + 7, eSettingsManager::ini->ReadString("Strings", "szKeyB", "B"));
		Patch<const char*>(0x42BD51 + 7, eSettingsManager::ini->ReadString("Strings", "szKeyC", "C"));
		Patch<const char*>(0x42BD5C + 7, eSettingsManager::ini->ReadString("Strings", "szKeyX", "X"));
		Patch<const char*>(0x42BD67 + 7, eSettingsManager::ini->ReadString("Strings", "szKeyY", "Y"));
		Patch<const char*>(0x42BD72 + 7, eSettingsManager::ini->ReadString("Strings", "szKeyZ", "Z"));
		Patch<const char*>(0x42BD7D + 7, eSettingsManager::ini->ReadString("Strings", "szKeyStart", "Start"));
		Patch<const char*>(0x407E43 + 1, eSettingsManager::ini->ReadString("Strings", "szSelectStageRandom", "Stage: Random"));
		Patch<const char*>(0x407E73 + 1, eSettingsManager::ini->ReadString("Strings", "szSelectStage", "Stage %i: %s"));
		Patch<const char*>(0x41A591 + 1, eSettingsManager::ini->ReadString("Strings", "szAppTitle ", "M.U.G.E.N"));
	}

	if (eSettingsManager::bGameModeTagShow)    Patch<char>(0x40AE10, 6);
	if (eSettingsManager::bGameModeSimulHide)  Patch<char>(0x40ADE4, 4);
	if (eSettingsManager::bGameModeSingleHide) Patch<char>(0x40ADC2, 4);
	if (eSettingsManager::bGameModeTurnsHide)  Patch<char>(0x40AE3C, 4);

	eStageAnnouncer::Init();

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
BOOL APIENTRY DllMain( HMODULE hModule,
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
