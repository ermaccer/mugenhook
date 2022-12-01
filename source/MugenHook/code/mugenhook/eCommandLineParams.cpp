#include "eCommandLineParams.h"
#include <Windows.h>
#include <shellapi.h>
#include <string>

bool CommandLine::ArgumentExists(wchar_t* arg)
{
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	static char argData[256] = {};
	if (argv)
	{
		for (int i = 0; i < argc; i++)
		{
			if (wcscmp(argv[i], arg) == 0)
			{
				LocalFree(argv);
				return true;
			}
		}
	}

	LocalFree(argv);

	return false;
}

char* CommandLine::GetStringArgument(wchar_t* arg)
{
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	static char argData[256] = {};
	if (argv)
	{
		for (int i = 0; i < argc; i++)
		{
			if (wcscmp(argv[i], arg) == 0)
			{
				std::wstring wstr = argv[i + 1];
				std::string str("", wstr.length());
				std::copy(wstr.begin(), wstr.end(), str.begin());
				sprintf(argData, str.c_str());
			}
		}
	}

	LocalFree(argv);

	return argData;
}
