#include "eSelectScreenManager.h"
#include <iostream>
#include "eMugen.h"
#include "..\stdafx.h"

int iPal_p1, iPal_p2;

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

