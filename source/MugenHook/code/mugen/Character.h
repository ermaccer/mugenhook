#pragma once

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

struct eMugenCharacterData {
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

struct eMugenCharacterObjectInfo {
	char pad[48];
	char RealName[48];
};


struct eMugenCharacter {
	eMugenCharacterObjectInfo* Info;
	char                 pad[4];
	int					 Team;
	char				 _pad[20];
	char                 ScriptName[48];
	int                  MaxHealth;
	int                  MaxPower;
	int                  Attack;
	char				 __pad[348];
	int					 Health;
	char				 ___pad[28];
	int					 Power;
};

eMugenCharacterInfo* GetCharInfo(int id);