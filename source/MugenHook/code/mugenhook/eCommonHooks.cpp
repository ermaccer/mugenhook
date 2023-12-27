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
#include "..\mugen\Draw.h"
#include "..\core\eCursor.h"
#include "eVariationsManager.h"
#include "eMagicBoxes.h"
#include "eAnimatedIcons.h"
#include "eTextDraw.h"


void eCommonHooks::Init()
{	
	InjectHook(0x429B14, HookDrawMugenVersionInfo, PATCH_CALL);
	InjectHook(0x4089A4, HookSelectScreenProcessEnd, PATCH_JUMP);
	InjectHook(0x40D213, HookDrawHUDLow, PATCH_JUMP);
	InjectHook(0x40D84D, HookDrawHUDHigh, PATCH_JUMP);
	InjectHook(0x40D663, HookDrawHUDMedium, PATCH_JUMP);
	InjectHook(0x40D255, HookDrawAfterHUDLow, PATCH_CALL);
	InjectHook(0x40D675, HookDrawAfterHUDMedium, PATCH_CALL);
	InjectHook(0x40D862, HookDrawAfterHUDHigh, PATCH_CALL);
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

void __declspec(naked) eCommonHooks::HookDrawHUDLow()
{
	static int jmp_continue = 0x40D219;
	_asm {
		pushad
	}
	eTextDrawProcessor::DrawTextDraws(0);
	_asm {
		popad
		mov     eax, [ebp + 8]
		cmp     dword ptr[eax], 4
		jmp jmp_continue
	}
}

void  __declspec(naked) eCommonHooks::HookDrawHUDMedium()
{
	static int jmp_continue = 0x40D66A;
	_asm {
		pushad
	}
	eTextDrawProcessor::DrawTextDraws(1);
	_asm {
		popad
		cmp     dword ptr[esi + 0x12854], 0
		jmp jmp_continue
	}
}

void __declspec(naked) eCommonHooks::HookDrawHUDHigh()
{
	static int jmp_continue = 0x40D853;
	_asm {
		pushad
	}
	eTextDrawProcessor::DrawTextDraws(2);
	_asm {
		popad
		mov     eax, [ebp + 8]
		cmp     dword ptr[eax], 4
		jmp jmp_continue
	}
}

void eCommonHooks::HookDrawAfterHUDLow(int unk)
{
	eTextDrawProcessor::DrawTextDraws(3);
	DrawFightDef(unk);
	eTextDrawProcessor::DrawTextDraws(6);
}

void eCommonHooks::HookDrawAfterHUDMedium(int unk)
{
	eTextDrawProcessor::DrawTextDraws(4);
	DrawFightDef(unk);
	eTextDrawProcessor::DrawTextDraws(7);
}

void eCommonHooks::HookDrawAfterHUDHigh(int unk)
{
	eTextDrawProcessor::DrawTextDraws(5);
	DrawFightDef(unk);
	eTextDrawProcessor::DrawTextDraws(8);
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

