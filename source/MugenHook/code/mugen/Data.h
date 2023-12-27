#pragma once
#include "Character.h"


enum eSlotIDs {
	SLOT_1 = 1,
	SLOT_2,
	SLOT_3,
	SLOT_4
};

enum eTeamIDs{
	TEAM_1 = 1,
	TEAM_2
};


struct eStageData {
	char pad[928];
	char BGMusic[512];
	char _pad[16];
	int  BGMVolume;
	int  BGMLoop;
	int  BGMLoopStart;
	int  BGMLoopEnd;


};

class eMugenData {
public:
	char  pad[256];
	char  CacheMusic[256];
	char  GameFolder[512];
	char  _pad[16];
	char  LastStage[1024]; // 2064
	char __pad[4816];
	eStageData StageData;

	int GetTeamCharactersAmount(int team);
	int GetMatch();
	int GetTotalMatches();
	int GetRound();
	int GetWinningSide();

	eMugenCharacter* GetCharacter(int id);
	eMugenCharacter* GetCharacter(int team, int slot);
	int				 GetIDFromObject(int object);

	void PrintAllCharacters();


	void RecalculateBGMVolume();

};

void LoadStage(eStageData* data);