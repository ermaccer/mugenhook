#pragma once
#pragma once
#include <iostream>
#include <cstdarg>

class eLog {
private:
	static FILE* pFile;
	static std::string szLastMessage;
	static std::string szLastFunction;
public:
	static void Open(const char* name);
	static void PushMessage(const char* function, const char* format, ...);
	static void PushError();
};

