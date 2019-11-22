#include "eLog.h"
#include "eSettingsManager.h"
#include "..\stdafx.h"

eLog* Log = new eLog();
void eLog::Open(const char * name)
{
	if (SettingsMgr->bUseLog)
	{
		pFile = fopen(name, "w");

		if (!pFile)
			printf("eLog::Open() | Failed to open %s!", name);
	}
}

void eLog::PushMessage(bool con ,const char * format, ...)
{
	char text[1024];
	va_list va;
	va_start(va, format);
	vsnprintf(text, 1024, format, va);
	va_end(va);
	std::string msg(text, strlen(text));
	szLastMessage = msg;
	if (!SettingsMgr->bUseLog) printf(text);
	else 
	if (pFile)
	{
		fprintf(pFile, text);
	}

}

void eLog::PushError()
{
	MessageBoxA(0, szLastMessage.c_str(), "MugenHook Error", 0);
}
