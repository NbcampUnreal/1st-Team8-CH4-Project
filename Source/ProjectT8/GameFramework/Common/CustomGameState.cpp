#include "CustomGameState.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Common/T8GameInstance.h"
#include "Global/Managers/UIManager.h"


void ACustomGameState::BeginPlay()
{
    Super::BeginPlay();

}

void ACustomGameState::SetGamePhase(EGamePhase _NewPhase)
{
    if (CurPhase != _NewPhase)
    {
        CurPhase = _NewPhase;
        OnPhaseChanged.Broadcast(_NewPhase);
        HandlePhaseChanged(_NewPhase);
    }
}

void ACustomGameState::HandlePhaseChanged_Implementation(EGamePhase _NewPhase)
{
    if (UT8GameInstance* GI = Cast<UT8GameInstance>(GetGameInstance()))
    {
        if (UUIManager* UIManager = GI->GetSubsystem<UUIManager>())
        {
            UIManager->ShowUI(_NewPhase);
        }
    }
}