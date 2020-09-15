#include "eLog.h"
#include "..\mugenhook\base.h"
#include <iostream>
#include <Windows.h>

std::string eLog::szLastMessage;
std::string eLog::szLastFunction;
FILE* eLog::pFile;



void eLog::Open(const char * name)
{
	pFile = fopen(name, "w");

	if (!pFile)
		printf("eLog::Open() | Failed to open %s!", name);
	else
		fprintf(pFile, "MugenHook %s Log Started\n", MUGENHOOK_VERSION);
}

void eLog::PushMessage(const char * function, const char * format, ...)
{
	char text[1024];
	va_list va;
	va_start(va, format);
	vsnprintf(text, 1024, format, va);
	va_end(va);
	std::string msg(text, strlen(text));
	szLastMessage = msg;
	szLastFunction = function;
	std::string functionName = function;
	functionName += " | ";
	msg.insert(0, functionName);
	printf(msg.c_str());

	if (pFile)
		fprintf(pFile, msg.c_str());

}

void eLog::PushError()
{
	MessageBox(0, szLastMessage.c_str(), szLastFunction.c_str(), MB_ICONERROR);
}
