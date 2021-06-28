#include "eScriptProcessor.h"
#include "base.h"

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
