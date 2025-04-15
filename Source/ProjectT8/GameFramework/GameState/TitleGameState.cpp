#include "TitleGameState.h"


void ATitleGameState::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle DelayHandle;
    GetWorld()->GetTimerManager().SetTimer(
        DelayHandle, 
        this, 
        &ATitleGameState::InitPhaseAfterDelay, 
        0.1f, 
        false);
}

void ATitleGameState::InitPhaseAfterDelay()
{
    SetGamePhase(EGamePhase::Title);
}