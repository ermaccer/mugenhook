#include "Character.h"
#include "System.h"

eMugenCharacterInfo* GetCharInfo(int id)
{
    eMugenCharacterInfo* CharactersArray = *(eMugenCharacterInfo**)0x503394;
    for (int i = 0; i < eSystem::GetCharactersAmount(); i++)
    {
        if (CharactersArray[i].ID == id)
        {
            return &CharactersArray[i];
        }
    }
    return nullptr;
}
