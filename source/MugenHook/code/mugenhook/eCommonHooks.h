#pragma once

namespace eCommonHooks {
	void Init();
	void ProcessSelectScreen();
	void ProcessCursorNegative();
	void ProcessCursorPositive();
	void ProcessCursorDown();
	void ProcessCursorUp();
	void ProcessCommonCursorEvent();
	void HookDrawMugenVersionInfo(const char * message, int a2, int a3, int a4, int a5, char r, char g, char b);
	void ProcessCharacterSpriteEvent();
	void ProcessSelectScreenEvent();
	void HookPushDebugMessage(const char* message);

}