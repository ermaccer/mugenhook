#pragma once

#define INPUT_ACTION_UP      0x1
#define INPUT_ACTION_RIGHT   0x2
#define INPUT_ACTION_DOWN    0x4
#define INPUT_ACTION_LEFT    0x8
#define INPUT_ACTION_A       0x10
#define INPUT_ACTION_B       0x20
#define INPUT_ACTION_C       0x40
#define INPUT_ACTION_X       0x80
#define INPUT_ACTION_Y       0x100
#define INPUT_ACTION_Z       0x200
#define INPUT_ACTION_START   0x400

class eInputManager {
public:
	static int pInputManagerPtr;
	static int pInputManagerPtr_P2;

	static void   Init();
	static void   HookInputManager();
	static int    GrabCurrentAction();
	static bool   GetKeyAction(int keyAction);
	static bool   CheckLastPlayer();
};