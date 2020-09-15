#pragma once

enum eGameplayModes {
	MODE_ARCADE,
	MODE_VERSUS,
	MODE_TEAM_ARCADE,
	MODE_TEAM_VERSUS,
	MODE_TEAM_COOP,
	MODE_SURVIVAL,
	MODE_SURVIVAL_COOP,
	MODE_TRAINING,
	MODE_WATCH
};

enum eGameModes {
	MODE_SINGLE,
	MODE_SIMUL,
	MODE_TAG,
	MODE_TURNS
};


enum eGameFlowType {
	FLOW_INTRO = 3,
	FLOW_MENU,
	FLOW_OPTIONS,
	FLOW_RESET_VARS,
	FLOW_SELECT_SCREEN,
	FLOW_8,
	FLOW_9,
	FLOW_VERSUS,
	FLOW_IN_GAME
};



struct eMugenCharacter {
	int   ID;
	char  CharacterFlag;
	char  CurrentVariation;
	char  CurrentIconIndex;
	char  ExtraFlags;
	char  Name[48];
	char  FileName[512];
	char  FolderName[512];
	int   SpritePointer;
	float Scale;
};


struct eMugenData {
	char  pad[512];
	char  GameFolder[512];
	char  _pad[16];
	char  LastStage[1024];

};


class eSystem {
public:
	static int iRows;
	static int iColumns;
	static int iSoundP1DoneGroup;
	static int iSoundP1DoneIndex;

	static int iSoundP2DoneGroup;
	static int iSoundP2DoneIndex;

	static int iPortraitGroup; 
	static int iPortraitIndex;

	static int pMugenResourcesPointer;
	static int pMugenCharactersPointer;
	static int pMugenDataPointer;

    // system def stuff
	static float p1_face_offset[2];
	static float p2_face_offset[2];

	static void Init();


	static int GetGameFlow();
	static int GetGameplayMode();
	static int GetTimer();
};