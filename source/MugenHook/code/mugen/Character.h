#pragma once
#include "System.h"

struct eMugenCharacter {
	eMugenCharacterInfo* Info;
	char                 pad[28];
	char                 ScriptName[48];
	int                  MaxHealth;
	int                  MaxPower;
	int                  Attack;
	int                  Unknown;

};