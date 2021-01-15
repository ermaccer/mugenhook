#pragma once
#include <iostream>
#include <cstdarg>

class eLog {
private:
	FILE* pFile;
	std::string szLastMessage;
public:
	void Open(const char* name);
	void PushMessage(bool con,const char* format, ...);
	void PushError();
};

extern eLog* Log;