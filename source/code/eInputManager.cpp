#include "eInputManager.h"
#include "eMugen.h"
#include <iostream>
int pInputManagerPtr;
int pInputManagerEcx;
int jInputManagerJmp_false = 0x478B29;
int jInputManagerJmp_true = 0x478AAF;

void __declspec(naked) eInputManager::HookInputManager()
{
	_asm {
		mov pInputManagerEcx, esi
	}
	printf("Pressing %d               \r", *(int*)(pInputManagerEcx + 1616 - 2608));
	_asm {
		test ecx, 4095
		jz s_false
		push 0x478AAF
		retn

		s_false:
		push 0x478B29
		retn
	}
}
