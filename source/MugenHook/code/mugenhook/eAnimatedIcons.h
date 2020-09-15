#pragma once
#include <string>
#include <vector>
#include "..\mugen\System.h"

#define EXTRA_FLAG_ANIM_RESET 1
#define EXTRA_FLAG_ANIM_PLAYS_ONCE 2
#define EXTRA_FLAG_ANIM_PLAYS_LOOP 4
#define EXTRA_FLAG_VAR_DONE_ANIM 8
#define EXTRA_FLAG_VAR_SPECIAL_CONTROLLER 16
#define EXTRA_FLAG_VAR_SPECIAL_ONSET 32
struct eAnimIconEntry {
	int         CharacterID;
	int         AnimationID;
	std::string AirFileName;
	int         Flags;
};

class eAnimatedIcons {
public:
	static std::vector<eAnimIconEntry> m_vAnimatedIcons;
	static std::vector<int> m_vUsedCharacters;
	static bool m_bScanRequiredForAIcons;
	static int m_nIconcounter;


	static void   Init();
	static void   ReadFile(const char* file);
	static void   FlagCharacters();
	static int    FindIconEntry(int charID);
	static void   Animate(eMugenCharacter* character);
	static void   RefreshAnimationCounters();
};