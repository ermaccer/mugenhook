#pragma once
#include <iostream>
namespace eFightLogger {
	void Init(const char* name);
	void SaveInitialMatchData();
	void SaveMatchData();
	void HookSaveMatchDataOne();
	void HookSaveMatchDataTwo();
}