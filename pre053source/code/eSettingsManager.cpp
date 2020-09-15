#include "eSettingsManager.h"

eSettingsManager* SettingsMgr = new eSettingsManager();

void eSettingsManager::Init()
{
	ini = new CIniReader("");

	bMugenhookFirstRun = ini->ReadBoolean("Settings", "bFirstRun", 0);
	bUseLog = ini->ReadBoolean("Settings", "bEnableLogToFile", 0);
	bGameModeTagShow = ini->ReadBoolean("Settings", "bGameModeTagShow", 0);
	bGameModeSingleHide = ini->ReadBoolean("Settings", "bGameModeSingleHide", 0);
	bGameModeSimulHide = ini->ReadBoolean("Settings", "bGameModeSimulHide", 0);
	bGameModeTurnsHide = ini->ReadBoolean("Settings", "bGameModeTurnsHide", 0);
	bChangeStrings = ini->ReadBoolean("Settings", "bChangeStrings", 0);
	iSelectableFighters = ini->ReadInteger("Settings", "iSelectableFighters", 0);

	// select screen stuff
	bRandomStageConfirmSounds = ini->ReadBoolean("Settings", "bRandomStageConfirmSounds", 0);
	iRandomStageGroup = ini->ReadInteger("Settings", "iRandomStageGroup", 0);
	iRandomStageRandomMax = ini->ReadInteger("Settings", "iRandomStageRandomMax", 0);
	bDisableNumerationInStageSelect = ini->ReadBoolean("Settings", "bDisableNumerationInStageSelect", 0);
	bHookCursorTable = ini->ReadBoolean("Settings", "bHookCursorTable", 0);
	bEnableSlidePortraits = ini->ReadBoolean("Settings", "bEnableSlidePortraits", 0);
	bEnableTagFix = ini->ReadBoolean("Settings", "bEnableTagFix", false);
	bHookVariations = ini->ReadBoolean("Settings", "bHookVariations", false);
	bHideVariationCharacters = ini->ReadBoolean("Settings", "bHideVariationCharacters", false);
	bHookMagicBoxes = ini->ReadBoolean("Settings", "bHookMagicBoxes", false);
	bUseLeftRightInMenu = ini->ReadBoolean("Settings", "bUseLeftRightInMenu", false);
	iTagSelectableFighters = ini->ReadInteger("Settings", "iTagSelectableFighters", 2);

	// dev mode
	bDevMode = ini->ReadBoolean("Settings", "bDevMode", 0);
	bDev_DisplayPos = ini->ReadBoolean("Settings", "bDev_DisplayPos", 0);
	bDev_RedirectMugenMessages = ini->ReadBoolean("Settings", "bDev_RedirectMugenMessages", 0);
	bDumpCharacterInfo = ini->ReadBoolean("Settings", "bDumpCharacterInfo", 0);

	// animated ports
	bHookAnimatedPortraits = ini->ReadBoolean("Settings", "bHookAnimatedPortraits", 0);
	bEnableAlternateAnims = ini->ReadBoolean("Settings", "bEnableAlternateAnims", 0);
	bEnableSelectAnimations = ini->ReadBoolean("Settings", "bEnableSelectAnimations", 0);
	bEnableAnimationScale = ini->ReadBoolean("Settings", "bEnableAnimationScale", 0);

	bUseFightLog = ini->ReadBoolean("Settings", "bUseFightLog", 0);
	szFightLogFile = ini->ReadString("Settings", "szFightLogFile", 0);
}
