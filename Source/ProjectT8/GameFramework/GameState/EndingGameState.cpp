#include "EndingGameState.h"


void AEndingGameState::BeginPlay()
{
    Super::BeginPlay();

    SetGamePhase(EGamePhase::Ending);
}