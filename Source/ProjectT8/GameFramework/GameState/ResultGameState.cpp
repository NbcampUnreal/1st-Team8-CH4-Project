#include "ResultGameState.h"
#include "GameFramework/Common/T8GameInstance.h"


void AResultGameState::BeginPlay()
{
    Super::BeginPlay();

    SetGamePhase(EGamePhase::Result);

    UT8GameInstance* GI = Cast<UT8GameInstance>(GetGameInstance());
    if (GI)
    {
        SetWinningPlayerStates(GI->WinningPlayerStatesResult);
    }
}

void AResultGameState::SetWinningPlayerStates(const TArray<APlayerState*>& NewWinningPlayerStates)
{
	WinningPlayerStates = NewWinningPlayerStates;
}
