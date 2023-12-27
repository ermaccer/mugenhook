#include "eSelectTimer.h"
#include "..\mugen\System.h"
#include "..\mugen\Draw.h"
#include "..\mugenhook\eMenuManager.h"
#include "..\core\eCursor.h"
#include <iostream>

int  eSelectTimer::ms_nTimer;
int  eSelectTimer::ms_nCurAmount;
bool eSelectTimer::ms_bForceSelection;
int eSelectTimer::ms_nCurPal = 0;
void eSelectTimer::Init()
{
	if (!eSystem::screentimer.active)
		return;

	ms_nCurAmount = eSystem::screentimer.amount;
	ms_nTimer = eSystem::GetTimer();
	ms_bForceSelection = false;
}

void eSelectTimer::ProcessTimer()
{
	if (eSystem::GetTimer() - ms_nTimer > eSystem::screentimer.ticks_per_sec) {
		if (ms_nCurAmount > 0)
			ms_nCurAmount--;
		ms_nTimer = eSystem::GetTimer();
	}

	if (ms_nCurAmount == 0)
		ms_bForceSelection = true;
}

void eSelectTimer::DrawTimer()
{
	static int font;
	if (!font)
		font = LoadFont(eSystem::screentimer.font, 1);

	eTextParams params = {};
	params.m_letterSpacingX = 1.0;
	params.m_letterSpacingY = 1.0;
	params.field_50 = 1.0;
	params.m_skew = 1.0;
	params.field_3C = 0.0;
	params.rotateY = 1.0;
	params.field_4 = 0.0;

	params.field_5C = 0xFFFF;
	params.m_alpha = 1;

	params.m_scaleX = eSystem::screentimer.scale_x;
	params.m_scaleY = eSystem::screentimer.scale_y;
	params.m_xPos = eSystem::screentimer.x;
	params.m_yPos = eSystem::screentimer.y;

	char sTextBuff[128];

	switch (eSystem::screentimer.num0s)
	{
	case 1:
		sprintf(sTextBuff,"%s%02d", eSystem::screentimer.format, ms_nCurAmount);
		break;
	case 2:
		sprintf(sTextBuff, "%s%03d", eSystem::screentimer.format, ms_nCurAmount);
		break;
	default:
		sprintf(sTextBuff, "%s%d", eSystem::screentimer.format, ms_nCurAmount);
		break;
	}

	Draw2DText(sTextBuff, font, &params, eSystem::screentimer.color_r, eSystem::screentimer.color_g, eSystem::screentimer.color_b, eSystem::screentimer.text_align);
}

void eSelectTimer::Process()
{
	if (!eSystem::screentimer.active)
		return;

	// we dont want timer in training or watch
	if (eSystem::GetGameplayMode() == MODE_TRAINING || eSystem::GetGameplayMode() == MODE_WATCH)
		return;

	// also we dont want mode selection
	if (eMenuManager::IsGameModeSelectOn())
		return;

	// if characters are selected don't show timer
	if (eMenuManager::AreCharactersSelected())
		return;

	ProcessTimer();

	if (strlen(eSystem::screentimer.format) > 0 || eSystem::screentimer.num0s >= 0)
		DrawTimer();

}

void __declspec(naked) eSelectTimer::ForceSelection_Hook()
{
	static int jmp_continue = 0x406827;
	static int jmp_end = 0x406827;
	if (ms_bForceSelection)
	{
		_asm {
			jmp jmp_end
		}
	}
	else
	{
		_asm {
			cmp     dword ptr[esp + 72], -1
			jmp		jmp_continue
		}
	}
}

void  __declspec(naked) eSelectTimer::ForceSelection_PalFix_Hook()
{
	static int jmp_continue = 0x406975;
	_asm {
		mov ms_nCurPal, eax
		pushad
	}
	if (ms_bForceSelection)
		ms_nCurPal = 0;
	_asm {
		popad
		lea     ecx, [ecx + ecx * 4]
		mov     eax, ms_nCurPal
		mov     [esi + ecx * 4], eax
		jmp jmp_continue
	}
}
