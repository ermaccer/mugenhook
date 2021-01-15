#include "eFightLogger.h"
#include "eLog.h"
#include "eMugen.h"
#include "eUtils.h"

#include <iostream>

FILE* pFightLog;

void eFightLogger::Init(const char* name)
{
	pFightLog = fopen(name, "w"	);
	if (!pFightLog)
	{
		Log->PushMessage(false, "eFightLogger::Init() | ERROR: Failed to open %s!\n", name);
		Log->PushError();
	}
}

void eFightLogger::SaveInitialMatchData()
{
	eMugenData* mugenData = *(eMugenData**)0x5040E8;
	fprintf(pFightLog, "[Match %d]\n", *(int*)(*(int*)MugenSystem->pMugenResourcesPointer + ResourceConvert10to11(71640)));
	fprintf(pFightLog, "totalmatches = %d\n", *(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x1057C - 48));//0x1057C - 48));
	fprintf(pFightLog, "stage = %s\n", mugenData->LastStage);

}

void eFightLogger::SaveMatchData()
{
	double time;
	int baseTime = *(int*)(*(int*)(MugenSystem->pMugenDataPointer + 0x11E98));
	printf("%f %f\n", time, baseTime);
	fprintf(pFightLog,"[Match %d Round %d]\n", *(int*)(*(int*)MugenSystem->pMugenResourcesPointer + ResourceConvert10to11(71640)), *(int*)(*(int*)MugenSystem->pMugenDataPointer + 0x12820));
	if (baseTime == -1) time = -1.0;
	else
		time = (double)baseTime / 60;

	fprintf(pFightLog, "timeleft =%.2f\n", (int)time >> 32);
}

void __declspec(naked) eFightLogger::HookSaveMatchDataOne()
{
	static int jmpContinue = 0x4153A7;
	SaveMatchData();
	_asm {
		mov edx, [ebx + 74868]
		jmp jmpContinue
	}
}

void eFightLogger::HookSaveMatchDataTwo()
{
	SaveMatchData();
	((void(__cdecl*)())0x410550)();

}
