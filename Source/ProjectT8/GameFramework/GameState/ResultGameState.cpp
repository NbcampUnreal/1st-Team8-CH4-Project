#include "ResultGameState.h"


void AResultGameState::BeginPlay()
{
    Super::BeginPlay();

    SetGamePhase(EGamePhase::Result);
}
