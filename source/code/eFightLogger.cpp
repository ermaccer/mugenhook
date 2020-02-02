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

void eFightLogger::HookLogMatchData()
{
	PushDebugMessage("End of match loop\n");
	LogMatchData();
}

void eFightLogger::LogMatchData()
{
	eMugenData* mugenData = *(eMugenData**)0x5040E8;
	fprintf(pFightLog, "[Match %d]\n", *(int*)(*(int*)MugenSystem->pMugenResourcesPointer + ResourceConvert10to11(71640)));
	fprintf(pFightLog, "totalmatches = %d\n", *(int*)(*(int*)MugenSystem->pMugenResourcesPointer + 0x1057C - 48));//0x1057C - 48));
	fprintf(pFightLog, "stage = %s\n", mugenData->LastStage);

}
