#include "eMugenConfig.h"
#include "..\mugen\System.h"

void eMugenConfig::SetCursorSound(int player, int group, int index)
{
	switch (player)
	{
	case 1:
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x340) = group;
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x348) = index;
		break;
	case 2:
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x344) = group;
		*(int*)(*(int*)eSystem::pMugenResourcesPointer + 0x34C) = index;
		break;
	default:
		break;
	}
}
