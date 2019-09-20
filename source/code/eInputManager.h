#pragma once

#define INPUT_BUTTON       16
#define INPUT_BUTTON_BLK   32
#define INPUT_BUTTON_LK    64
#define INPUT_BUTTON_HP    128
#define INPUT_BUTTON_RUN   256
#define INPUT_BUTTON_HK    512
#define INPUT_BUTTON_START 1024

namespace eInputManager {
	void HookInputManager();
}