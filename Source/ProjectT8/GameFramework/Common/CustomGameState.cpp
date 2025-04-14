#include "CustomGameState.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Common/T8GameInstance.h"
#include "Global/Managers/UIManager.h"


void ACustomGameState::SetGamePhase(EGamePhase _NewPhase)
{
    if (CurPhase != _NewPhase)
    {
        CurPhase = _NewPhase;
        OnPhaseChanged(_NewPhase);
    }
}

void ACustomGameState::OnPhaseChanged_Implementation(EGamePhase _NewPhase)
{
    if (UT8GameInstance* GI = Cast<UT8GameInstance>(GetGameInstance()))
    {
        if (UUIManager* UIManager = GI->GetSubsystem<UUIManager>())
        {
            UIManager->ShowUI(_NewPhase);
        }
    }
}