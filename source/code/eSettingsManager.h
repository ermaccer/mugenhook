#pragma once
#include "..\IniReader.h"
// updating to other plugins like pluginmh

class eSettingsManager {
public:
	void Init();
	CIniReader* ini;

	bool bMugenhookFirstRun;

	// vars
	bool bUseLog;
	bool bGameModeTagShow;
	bool bGameModeSingleHide;
	bool bGameModeSimulHide;
	bool bGameModeTurnsHide;
	bool bDisableNumerationInStageSelect;
	bool bChangeStrings;
	bool bRandomStageConfirmSounds;
	bool bDumpCharacterInfo;
	bool bDevMode;
	bool bDev_DisplayPos;
	bool bDev_RedirectMugenMessages;
	/* Cursor Manager */
	bool bHookCursorTable;
	int  iPlaybackWaitTime;

	/* Animated Portraits */
	bool bEnableSelectAnimations;
	bool bEnableAlternateAnims;
	bool bEnableAnimationScale;
	bool bHookAnimatedPortraits;

	/* Slide Portraits */
	bool bEnableSlidePortraits;

	/* Other */
	int  iSelectableFighters;
	int  iRandomStageGroup;
	int  iRandomStageRandomMax;
	bool bUseLeftRightInMenu;

	/* Tag Stuff */
	bool bEnableTagFix;
	int  iTagSelectableFighters;

	/* Variations */
	bool bHookVariations;
	bool bHideVariationCharacters;

	/* Magic Boxes */
	bool bHookMagicBoxes;

	/* Fight Logger*/
	bool        bUseFightLog;
	const char* szFightLogFile;
};

extern eSettingsManager* SettingsMgr;