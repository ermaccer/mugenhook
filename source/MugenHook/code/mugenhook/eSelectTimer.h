#pragma once

class eSelectTimer {
public:
	static int  ms_nTimer;
	static int  ms_nCurAmount;
	static bool ms_bForceSelection;
	static int  ms_nCurPal;

	static void Init();
	static void ProcessTimer();
	static void DrawTimer();
	static void Process();
	static void ForceSelection_Hook();
	static void ForceSelection_PalFix_Hook();
};