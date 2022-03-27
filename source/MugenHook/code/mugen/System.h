#pragma once
#include "Sound.h"

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
	FLOW_0,
	FLOW_1,
	FLOW_2,
	FLOW_INTRO,
	FLOW_MENU,
	FLOW_OPTIONS,
	FLOW_RESET_VARS,
	FLOW_SELECT_SCREEN,
	FLOW_8,
	FLOW_9,
	FLOW_VERSUS,
	FLOW_IN_GAME,
	FLOW_QUIT_LOOP = 20
};



struct eMugenCharacterInfo {
	int   ID;
	char  CharacterFlag;
	char  CurrentVariation;
	char  CurrentIconIndex;
	char  ExtraFlags;
	char  Name[48];
	char  FileName[512];
	char  FolderName[508]; // originally 512, but we need data for timer, don't think it'll exceed 260 anyway
	int   IconTimer;       // set with 0 anyway
	int   SpritePointer;
	float Scale;
};

struct eMugenStage {
	char FileName[512];
	char StageName[84];
};

struct eMugenCharacterData{
	int ID;
	int Unknown[2];
	char pad[512];
	char FileName[512];
	char FilePath[1536];
	char _pad[84]; // most likely pal assoc?
	char IntroFile[512];
	char EndingFile[512];
	char __pad[528];
};


struct eMugenData {
	char  pad[512];
	char  GameFolder[512];
	char  _pad[16];
	char  LastStage[1024];

};

struct eSelectScreenParams {
	char data[1200];
};

struct eGameFlowData {
	int status;
	eSelectScreenParams* params;
	char _pad[60];
	int  deinit;
	char __pad[32];
	int quit;
};

struct pushstart_settings {
	int active;
	char text[512] = {};
	char font[512] = {};
	int flash;
	int color_r;
	int color_g;
	int color_b;
	float scale_x;
	float scale_y;
	int p1_x, p1_y;
	int p2_x, p2_y;
	int p1_align, p2_align;
	int group, index;
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

	static int p1_cursor_startcell[2];
	static int p2_cursor_startcell[2];

	// join in settings

	static pushstart_settings pushstart_set;

	static void Init();

	static Sound* GetSystemSND();
	static int GetGameFlow();
	static int GetGameplayMode();
	static int GetTimer();
	static int GetCharactersAmount();

	static void SetGameFlow(eGameFlowType flow);
	static void SetGameplayMode(eGameplayModes mode);


	static void ClearScreenparams(eSelectScreenParams* params);
	static void SetScreenParams(eSelectScreenParams* params, int gameMode, int unk);
};