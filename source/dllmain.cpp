// mugenhook
// ermaccer
#include "code\eMugenHook.h"


extern "C"
{
	__declspec(dllexport) void InitializeASI()
	{
		Init();
	}
}

