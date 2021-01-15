#include "eSelectScreenManager.h"
#include <iostream>
#include "eMugen.h"
#include "..\stdafx.h"

int iPal_p1, iPal_p2;

int stringEax;
int jumpPointDone = 0x404060;
int jumpPointContinue = 0x404F6D;

int iSelScreenPtr = 0;
int jmpSelScreen = 0x407E37;

std::string lastSelectEntry;
eMugenCharacter* Character;

int eSelectScreenManager::HookStageNumeration(char* dest, char* format, int dummy, char* name)
{
	return sprintf(dest,format,name);
}

int eSelectScreenManager::HookPalRequest(char * a1, int a2, int a3, int a4, int a5)
{
	iPal_p1 = a4;
	return printf("Selected pal: %d at %X\n",iPal_p1,&iPal_p1);
}
int eSelectScreenManager::GetSelectedPal(int player)
{
	if (player == 2)
		return iPal_p2;
	else
		return iPal_p1;
}

void __declspec(naked) eSelectScreenManager::HookSelectIDs()
{
	_asm {
		lea eax, [esp + 320]
		mov stringEax, eax
		mov eax, [esp + 20]
		mov Character, eax
		pushad
	}
	lastSelectEntry = (char*)stringEax;


	if (lastSelectEntry == "randomselect")
	{
		Character->ID = -2;
		Character->SpritePointer = 0;

		_asm {
			popad
			jmp jumpPointContinue
		}
	}
	else if (lastSelectEntry == "magicbox")
	{
		Character->ID = -3;
		Character->SpritePointer = 0;
		sprintf(Character->Name, "Magic Box");
		_asm {
			popad
			jmp jumpPointContinue
		}
	}
    else
    {
    	_asm {
    		popad
    		jmp jumpPointDone
    	}
    }


}

void __declspec(naked) eSelectScreenManager::HookCharacterInit()
{
}
