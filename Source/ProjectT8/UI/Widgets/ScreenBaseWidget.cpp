#include "UI/Widgets/ScreenBaseWidget.h"

#include "Global/Managers/UIManager.h"

void UScreenBaseWidget::OnScreenActivated_Implementation()
{
    if (FadeInAnim)
    {
        PlayAnimation(FadeInAnim);
    }
}

void UScreenBaseWidget::OnScreenDeactivated_Implementation()
{
    if (FadeOutAnim)
    {
        BindToAnimationFinished(FadeOutAnim, FadeOutFinishedDelegate);
        PlayAnimation(FadeOutAnim);
    }
    else
    {
        HandleFadeOutFinished();
    }
}

void UScreenBaseWidget::BindFadeOutFinished()
{
    if (FadeOutAnim)
    {
        BindToAnimationFinished(FadeOutAnim, FadeOutFinishedDelegate);
    }
}

void UScreenBaseWidget::HandleFadeOutFinished()
{
    RemoveFromParent();

    if (UUIManager* UI = GetGameInstance()->GetSubsystem<UUIManager>())
    {
        UI->NotifyScreenRemoved(this);
    }
}