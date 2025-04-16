#include "LoadingGameState.h"


void ALoadingGameState::BeginPlay()
{
    Super::BeginPlay();

    SetGamePhase(EGamePhase::Loading);
}
