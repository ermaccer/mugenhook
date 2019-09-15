#include "eSelectScreenManager.h"
#include <iostream>
int jSelectScreenJmp = 0x404F6D;
int iSelectScreen_eax;
void __declspec(naked)eSelectScreenManager::HookSelectReader()
{
	_asm {
		mov iSelectScreen_eax, esp
		pushad
	}
	printf("%X\n", &iSelectScreen_eax);
	_asm {
		popad
		jmp jSelectScreenJmp
	}
}
