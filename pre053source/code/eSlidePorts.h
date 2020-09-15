#pragma once

namespace eSlidePorts {
	void ReadSettings();
	void Update();
	void UpdateP2();
	// hooks
	int HookSelectCase(); // case 7
	int HookMenuCase();   // case 4
	int HookGameLoop();  // get access to gameloop

	
}