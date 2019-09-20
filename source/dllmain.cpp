// mugenhook
// ermaccer
#include "code\eMugenHook.h"


extern "C"
{
	__declspec(dllexport) void InitializeASI()
	{
		if (*(char*)0x40B97B == 0 && *(char*)0x40B97D == 1 && *(char*)0x40B97F == 1)
			Init();
		else
			MessageBoxA(0, "Invalid M.U.G.E.N version! Only 1.1.0 supported!", "MugenHook", MB_ICONERROR);

	}
}

