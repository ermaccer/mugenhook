#include "eTagFix.h"
#include "eSettingsManager.h"
#include <iostream>
int commandName = 0;
int fighters = 0;
int jumpPoint = 0x455BFC;

void __declspec(naked) eTagFix::Hook()
{
	static int jumpP = 0x406411;
	_asm {
		mov eax, [esp + 68]
		mov eax, [eax + 36]
		mov[eax + 8], 2
		mov[eax + 16], 0
		mov[eax + 12], 2
		jmp jumpP
	}

}

void __declspec(naked) eTagFix::HookGameModeCommand()
{
	_asm {
		lea eax, [esp + 344]
		mov commandName, eax
	}

	if (*(char*)(commandName) == 't' && *(char*)(commandName + 1) == 'a' &&
		*(char*)(commandName + 2) == 'g' && *(char*)(commandName + 3) == 0)
	{
		_asm
		{
			mov eax, [esp + 1268]
			add edi, ebx
			mov[esp + 48], 2
			mov[eax], edi
			jmp jumpPoint
		}
	}

	else if (*(char*)(commandName) == 't' && *(char*)(commandName + 1) == 'u' && 
		*(char*)(commandName + 2) == 'r' && *(char*)(commandName + 3) == 'n' &&
		*(char*)(commandName + 4) == 's' && *(char*)(commandName + 5) == 0)
	{
		_asm
		{
			mov eax, [esp + 1268]
			add edi, ebx
			mov[esp + 48], 3
			mov[eax], edi
			jmp jumpPoint
		}
	}

}
