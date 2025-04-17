#include "UI/Widgets/Screen/LoadingScreenWidget.h"

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
                if (UUIManager* UI = GetGameInstance()->GetSubsystem<UUIManager>())
                {
                    UI->OpenLevelForPhase(EGamePhase::Playing);
                }
            }),
        1.5f,
        false
    );
}