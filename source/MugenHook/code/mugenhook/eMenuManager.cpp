#include "eMenuManager.h"
#include "base.h"
#include "eSlidingPortraits.h"
#include "..\mugen\Draw.h"
#include "..\core\eSettingsManager.h"
#include "eMagicBoxes.h"
#include "eVariationsManager.h"
#include "eAnimatedIcons.h"
#include "..\core\eCursor.h"
#include "eSelectTimer.h"
#include "eSelectScreenManager.h"
#include "eTextDraw.h"

bool eMenuManager::m_bIsInMainMenu;
bool eMenuManager::m_bIsInSelectScreen;
bool eMenuManager::m_bAnimsRequireRefresh;
bool eMenuManager::m_bWasCursorAdjusted;
int  eMenuManager::m_fntOptions;
int  eMenuManager::m_pSelectScreenDataPointer;

void eMenuManager::Init()
{
	m_bIsInMainMenu     = false;
	m_bIsInSelectScreen = false;
	m_bAnimsRequireRefresh = true;
	m_bWasCursorAdjusted = false;
	m_fntOptions = 0;
	m_pSelectScreenDataPointer = 0;

	InjectHook(0x4125B5, HookSelectScreenMenu, PATCH_CALL);
    InjectHook(0x412591, HookMainMenu,         PATCH_CALL);
	InjectHook(0x407CC6, HookSelectScreenProcess, PATCH_JUMP);
	InjectHook(0x4126B5, HookGameLoop,         PATCH_CALL);
	InjectHook(0x4125C1, HookBeginMatch, PATCH_CALL);
}


int eMenuManager::HookMainMenu()
{
	m_bIsInMainMenu = true;
	m_bIsInSelectScreen = false;
	m_bWasCursorAdjusted = false;
	eCursor::ClearSelections();
	eTextDrawProcessor::InitTextDraw();
	eSelectScreenManager::m_nSelectScreenCurrentCharacterDraw = 0;
	if (eSettingsManager::bHookVariations)
	{
		if (eVariationsManager::GetAmountOfUsedVariationCharacters() > 0)
		eVariationsManager::ResetVariationStatus();

	}


	if (eSettingsManager::bEnableSlidePortraits)
		eSlidingPortraits::Reset();

	if (eSettingsManager::bHookMagicBoxes)
	{
		eMagicBoxes::GetBox();
		eMagicBoxes::ResetBoxesStatus();
	}

	if (eSettingsManager::bHookAnimatedIcons)
		eAnimatedIcons::FlagCharacters();

	if (m_bAnimsRequireRefresh)
	{
		if (eSettingsManager::bHookAnimatedIcons)
		{
			eAnimatedIcons::RefreshAnimationCounters();
			m_bAnimsRequireRefresh = false;
		}
	}
	return CallAndReturn<int, 0x429C20>();
}

int eMenuManager::HookSelectScreenMenu()
{
	m_bIsInMainMenu = false;
	m_bIsInSelectScreen = true;
	m_bAnimsRequireRefresh = true;
	eCursor::PopCursor();
	eCursor::ClearSelections();
	eSelectTimer::Init();
	eTextDrawProcessor::InitTextDraw();
	return CallAndReturn<int, 0x408A80>();
}

int eMenuManager::HookGameLoop()
{
	return CallAndReturn<int, 0x412FA0>();
}

int eMenuManager::HookBeginMatch()
{
	if (eSettingsManager::bEnableSlidePortraits)
		eSlidingPortraits::Reset();
	eTextDrawProcessor::InitTextDraw();

	return CallAndReturn<int, 0x4227D0>();
}

void __declspec(naked) eMenuManager::HookSelectScreenProcess()
{
	static int sel_proc_continue = 0x407CCD;

	_asm {
		mov     eax, [ebp + 12]
		mov		m_pSelectScreenDataPointer, eax
		mov     esi, [esp + 32]
		pushad
		popad
		jmp sel_proc_continue
	}
}

bool eMenuManager::IsGameModeSelectOn()
{
	if (!m_pSelectScreenDataPointer)
		return false;

	return *(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3BA4) || *(int*)(eMenuManager::m_pSelectScreenDataPointer + 0x3BA4 + 180);
}

bool eMenuManager::AreCharactersSelected()
{
	if (eCursor::Player2_Character < 0 && eCursor::Player1_Selected)
		return true;
	if (eCursor::Player1_Character < 0 && eCursor::Player2_Selected)
		return true;
	if (eCursor::Player1_Selected && eCursor::Player2_Selected)
		return true;

	return false;
}


