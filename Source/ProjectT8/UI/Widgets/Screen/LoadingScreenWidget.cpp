#include "UI/Widgets/Screen/LoadingScreenWidget.h"

#include "GameFramework/PlayerController.h"

#include "Global/Managers/UIManager.h"


void ULoadingScreenWidget::BindProgressBarFinished()
{
    if (Anim_ProgressBar_Loading)
    {
        BindToAnimationFinished(Anim_ProgressBar_Loading, ProgressBarFinishedDelegate);
    }
}

void ULoadingScreenWidget::HandleProgressBarFinished()
{
    //RemoveFromParent();

    FTimerHandle DelayHandle;
    GetWorld()->GetTimerManager().SetTimer(
        DelayHandle,
        FTimerDelegate::CreateLambda([this]()
            {
                /*if (UUIManager* UI = GetGameInstance()->GetSubsystem<UUIManager>())
                {
                    UI->OpenLevelForPhase(EGamePhase::Playing);
                }*/

                APlayerController* OwningController = GetOwningPlayer();
                if (OwningController && OwningController->GetLocalRole() == ENetRole::ROLE_Authority)
                {
                    UWorld* World = GetWorld();
                    World->ServerTravel("BattleLevel");
                }
            }),
        1.5f,
        false
    );
}