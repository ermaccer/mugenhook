#pragma once
#include "..\mugenhook\base.h"

class eSettingsManager {
public:

	static CIniReader* ini;

	static bool bMugenhookFirstRun;

	// vars
	static bool bUseLog;
	static bool bGameModeTagShow;
	static bool bGameModeSingleHide;
	static bool bGameModeSimulHide;
	static bool bGameModeTurnsHide;
	static bool bDisableNumerationInStageSelect;
	static bool bChangeStrings;
	static bool bRandomStageConfirmSounds;
	static bool bDumpCharacterInfo;
	static bool bDevMode;
	static bool bDev_DisplayPos;
	static bool bDev_RedirectMugenMessages;
	/* Cursor Manager */
	static bool bHookCursorTable;
	static bool bEnableAfterSelectionPause;
	static int  iAfterSelectionPauseTime;
	static int  iStartingGameModePos;
	static int  iCursorTableOperationType;

	/* Animated Portraits */
	static bool bEnableSelectAnimations;
	static bool bEnableAlternateAnims;
	static bool bEnableAnimationScale;
	static bool bHookAnimatedPortraits;
	static bool bEnableAnimationLoop;

	/* Slide Portraits */
	static bool bEnableSlidePortraits;

	/* Other */
	static int  iSelectableFighters;
	static int  iRandomStageGroup;
	static int  iRandomStageRandomMax;
	static bool bUseLeftRightInMenu;
	static bool bHookStageAnnouncer;

	/* Tag Stuff */
	static bool bEnableTagFix;
	static int  iTagSelectableFighters;

	/* Variations */
	static bool bHookVariations;
	static bool bHideVariationCharacters;

	/* Magic Boxes */
	static bool bHookMagicBoxes;

	/* Fight Logger*/
	static bool        bUseFightLog;
	static char*       szFightLogFile;

	/* Animated Icons*/
	static bool       bHookAnimatedIcons;

	static void Init();
};

