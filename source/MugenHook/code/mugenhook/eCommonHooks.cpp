#include "eCommonHooks.h"
#include "eCustomCursorManager.h"
#include "eSelectScreenManager.h"
#include "..\mugen\Common.h"
#include "..\core\eInputManager.h"
#include "eAnimatedPortraits.h"
#include "eSlidingPortraits.h"
#include "..\core\eSettingsManager.h"
#include "base.h"
#include "..\mugen\System.h"
#include "..\core\eCursor.h"
#include "eVariationsManager.h"
#include "eMagicBoxes.h"
#include "eAnimatedIcons.h"


void eCommonHooks::Init()
{	
	InjectHook(0x429B14, HookDrawMugenVersionInfo, PATCH_CALL);
	InjectHook(0x4089A4, HookSelectScreenProcessEnd, PATCH_JUMP);
}

void eCommonHooks::ProcessSelectScreen()
{
	eCustomCursorManager::Process();
	eSelectScreenManager::Process();

	if (eSettingsManager::bHookVariations)
	{
		eVariationsManager::HideVariationCharacters();
		eVariationsManager::UpdateCharactersP1();
		eVariationsManager::UpdateCharactersP2();
		eVariationsManager::ProcessInactivity();
	}

}

void eCommonHooks::ProcessCursorNegative()
{
	ProcessCommonCursorEvent();

}

void eCommonHooks::ProcessCursorPositive()
{
	ProcessCommonCursorEvent();
}

void eCommonHooks::ProcessCursorDown()
{
	ProcessCommonCursorEvent();
}

void eCommonHooks::ProcessCursorUp()
{
	ProcessCommonCursorEvent();
}

void eCommonHooks::ProcessCommonCursorEvent()
{
	if (!eInputManager::CheckLastPlayer())
	{
		eAnimatedPortraits::ResetFrameCounter(1);

		if (eSettingsManager::bEnableSlidePortraits)
		{
			if (eSlidingPortraits::portrait_slide_reset_on_move)
			{
				if (eSystem::GetGameplayMode() == MODE_TRAINING || eSystem::GetGameplayMode() == MODE_WATCH)
				{
					if (eCursor::Player1_Selected)
						eSlidingPortraits::ResetPlayer(2);
					else
						eSlidingPortraits::ResetPlayer(1);
				}
				else
					eSlidingPortraits::ResetPlayer(1);
			}
		}

			
	}
	else
	{
		eAnimatedPortraits::ResetFrameCounter(2);
		if (eSettingsManager::bEnableSlidePortraits)
		{
			if (eSlidingPortraits::portrait_slide_reset_on_move)
				eSlidingPortraits::ResetPlayer(2);
		}
	}
}


void eCommonHooks::HookDrawMugenVersionInfo(const char * message, int a2, int a3, int a4, int a5, char r, char g, char b)
{
	char temp[256];
	sprintf(temp, "%s | MugenHook %s by ermaccer", message, MUGENHOOK_VERSION);
	DrawScreenMessage(temp, a2, a3, a4, a5, r, g, b);
}

void eCommonHooks::ProcessCharacterSpriteEvent()
{
	if (eSettingsManager::bEnableSlidePortraits)
	{
		eSlidingPortraits::Process();
		eSlidingPortraits::ProcessP2();
	}

	if (eSettingsManager::bHookMagicBoxes)
		eMagicBoxes::Process();

	//eAnimatedIcons::FlagCharacters();
}

void eCommonHooks::HookPushDebugMessage(const char * message)
{
	printf(message);
}


void __declspec(naked) eCommonHooks::HookSelectScreenProcessEnd()
{
	static int end_jmp = 0x4089AB;
	_asm
	{
		pushad
	}

	eSelectScreenManager::ProcessEnd();

	_asm
	{
		popad
		pop edi
		pop esi 
		mov eax, 1
		jmp end_jmp
	}
}

