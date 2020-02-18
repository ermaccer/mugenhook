#include "eCharFlagsManager.h"
#include <Windows.h>
int eCharFlagsManager::ProcessHiddenCharacters(int* a1, int a2, int a3, int a4, int a5, int a6, int a7)
{	
	eMugenCharacter* character;
	_asm mov character, ecx
	if (!(character->CharacterFlag & CHAR_FLAG_HIDDEN))
		return ((int(__cdecl*)(int*, int, int, int, int, int, int))0x411C00)(a1, a2, a3, a4, a5, a6, a7);
	else
		return -1;
}
