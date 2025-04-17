#include "UI/Widgets/Screen/LoadingScreenWidget.h"

#include "Global/Managers/UIManager.h"


void ULoadingScreenWidget::OnScreenActivated_Implementation()
{
    Super::OnScreenActivated_Implementation();

    if (Anim_ProgressBar_Loading)
    {
        BindProgressBarFinished();
        PlayAnimation(Anim_ProgressBar_Loading);
    }
}

void ULoadingScreenWidget::BindProgressBarFinished()
{
    if (Anim_ProgressBar_Loading)
    {
        BindToAnimationFinished(Anim_ProgressBar_Loading, ProgressBarFinishedDelegate);
    }
}

void ULoadingScreenWidget::HandleProgressBarFinished()
{
    RemoveFromParent();

    if (UUIManager* UI = GetGameInstance()->GetSubsystem<UUIManager>())
    {
        UI->NotifyScreenRemoved(this);
    }
}