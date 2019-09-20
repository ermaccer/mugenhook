#pragma once

namespace eSelectScreenManager {
	int HookStageNumeration(char* dest, char* format, int dummy, char* name);
	int  HookPalRequest(char* a1, int a2, int a3, int a4, int a5);
	int  GetSelectedPal(int player);
}