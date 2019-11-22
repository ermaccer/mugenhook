#pragma once

// mugen data

struct eMugenCharacter {
	int   ID;
	int   pad;
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

struct eMugenSystem {
	int iRows, iColumns;
	int iSoundP1DoneGroup, iSoundP1DoneIndex;
	int iSoundP2DoneGroup, iSoundP2DoneIndex;
	int pMugenResourcesPointer;
	int pMugenCharactersPointer;
	int pMugenDataPointer;
};


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

namespace eMugenManager {
	void Init();
	int  GetTimer();
	int  GetGameFlow();
	int  GetGameplayMode();
}

// mugen functions
int RequestSprites(int param, int sprite_indexes);
int DrawSprites(int a1, int a2, int a3, int a4, int a5, float a6, float a7);
void ScaleSprites(int ptr, float a1, float a2);
int DrawScreenMessage(const char* message, int a2, int a3, int a4, int a5, char r, char g, char b);
void PushDebugMessage(const char* message);
void PlaySound(int sound, int unk, int id, int unk2, double unkd);
int  GetButtonID(int ptr);


// hooks
int  HookDrawScreenMessage(const char* message, int a2, int a3, int a4, int a5, char r, char g, char b);
void HookPushDebugMessage(const char* message);
void HookSoundPlayback(int a1,int a2, int a3,int a4, double a5);


extern eMugenSystem* MugenSystem;