#include "eScriptProcessor.h"
#include "base.h"
#include "..\core\eInputManager.h"
#include "..\mugen\System.h"
#include "..\mugen\Draw.h"
#include "..\core\eCursor.h"
#include "..\mugen\System.h"
#include "eSelectScreenManager.h"

void eScriptProcessor::Init()
{
	InjectHook(0x40DF2D, eScriptProcessor::Hook2DDraw, PATCH_JUMP);
}

void __declspec(naked) eScriptProcessor::Hook2DDraw()
{
	static int draw_continue = 0x40DF32;
	ProcessCustomDraws();

	_asm {
		mov eax, ds: 0x5040E8
		pushad
		popad
		jmp  draw_continue
	}
}

void eScriptProcessor::ProcessCustomDraws()
{
}

int eScriptProcessor::GetScreenResX()
{
	return *(int*)0x5050E8;
}
