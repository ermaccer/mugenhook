#include "eInputManager.h"
#include "..\mugenhook\base.h"

int eInputManager::pInputManagerPtr;
int eInputManager::pInputManagerPtr_P2;


void eInputManager::Init()
{
	pInputManagerPtr = 0;
	pInputManagerPtr_P2 = 0;

	InjectHook(0x406042, HookInputManager, PATCH_JUMP);
}


void __declspec(naked) eInputManager::HookInputManager()
{
	static int input_jmp = 0x406049;
	_asm {
		mov     [esp + 64], ecx
		mov     ecx, [ebp + 8]
		mov     esi, [ecx + 96]
	    mov     pInputManagerPtr, esi
		mov     esi, [ecx + 100]
		mov     pInputManagerPtr_P2, esi
		jmp input_jmp
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

