#include "RoomGameMode.h"
#include "Player/CharacterBase.h"
#include "GameFramework/Common/T8PlayerState.h"

ARoomGameMode::ARoomGameMode()
{
    DefaultPawnClass = ACharacterBase::StaticClass();
}
