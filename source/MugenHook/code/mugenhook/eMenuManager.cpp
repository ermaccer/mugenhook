#include "eMenuManager.h"
#include "base.h"
#include "eSlidingPortraits.h"
#include "..\mugen\Draw.h"
#include "..\core\eSettingsManager.h"
#include "eMagicBoxes.h"
#include "eVariationsManager.h"
#include "eAnimatedIcons.h"

bool eMenuManager::m_bIsInMainMenu;
bool eMenuManager::m_bIsInSelectScreen;
bool eMenuManager::m_bAnimsRequireRefresh;
int  eMenuManager::m_fntOptions;

void eMenuManager::Init()
{
	m_bIsInMainMenu     = false;
	m_bIsInSelectScreen = false;
	m_bAnimsRequireRefresh = true;
	m_fntOptions = 0;

	InjectHook(0x4125B5, HookSelectScreenMenu, PATCH_CALL);
    InjectHook(0x412591, HookMainMenu,         PATCH_CALL);
	//InjectHook(0x4126B5, HookGameLoop,         PATCH_CALL);
}

int eMenuManager::HookMainMenu()
{
	m_bIsInMainMenu = true;
	m_bIsInSelectScreen = false;

	if (eSettingsManager::bEnableSlidePortraits)
	    eSlidingPortraits::Reset();

	if (eSettingsManager::bHookMagicBoxes)
	{
		eMagicBoxes::GetBox();
		eMagicBoxes::ResetBoxesStatus();
	}

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
	return CallAndReturn<int, 0x408A80>();
}

int eMenuManager::HookGameLoop()
{
	return CallAndReturn<int, 0x412FA0>();
}
