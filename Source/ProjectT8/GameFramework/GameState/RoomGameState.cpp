#include "RoomGameState.h"


void ARoomGameState::BeginPlay()
{
    Super::BeginPlay();

    SetGamePhase(EGamePhase::Room);
}
