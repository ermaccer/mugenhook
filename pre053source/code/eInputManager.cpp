#include "eInputManager.h"
#include "eMugen.h"
#include <iostream>
int pInputManagerPtr = 0;
int pInputManagerPtr_P2 = 0;

void __declspec(naked) eInputManager::HookInputManager()
{
	static int jmpJumpPoint = 0x40604C;
	_asm {
		mov ecx, [ebp + 8]
		mov esi, [ecx + 88]
		mov eax, [ecx + 96]
		mov pInputManagerPtr, eax
		mov eax, [ecx + 100]
		mov pInputManagerPtr_P2, eax
		pushad
		popad
		jmp jmpJumpPoint
	}
}

int eInputManager::GrabCurrentAction()
{
	   return pInputManagerPtr;
}


bool eInputManager::GetKeyAction(int keyAction)
{
	if (pInputManagerPtr & keyAction)
		return true;
	else
		return false;

}

bool eInputManager::CheckLastPlayer()
{
	if (pInputManagerPtr_P2 == 1)
		return true;
	else
    	return false;
}

