// mugenhook
// ermaccer

#include "hooks.h"

extern "C"
{
	__declspec(dllexport) void InitializeASI()
	{
		eHooks::Init();
	}
}

