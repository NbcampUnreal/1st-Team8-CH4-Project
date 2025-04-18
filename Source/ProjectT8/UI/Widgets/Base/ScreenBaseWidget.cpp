#include "UI/Widgets/Base/ScreenBaseWidget.h"

#include "Global/Managers/UIManager.h"

void UScreenBaseWidget::OnScreenActivated_Implementation()
{
    // FadeIn �ִϸ��̼� ���
    if (Anim_FadeIn_Screen)
    {
        PlayAnimation(Anim_FadeIn_Screen);
    }
}

void UScreenBaseWidget::OnScreenDeactivated_Implementation()
{
    // FadeOut �ִϸ��̼� Delegate ���� ��, ���
    if (Anim_FadeOut_Screen)
    {
        PlayAnimation(Anim_FadeOut_Screen);
    }
    else
    {
        HandleFadeOutFinished();
    }
}

void UScreenBaseWidget::BindFadeOutFinished()
{
    if (Anim_FadeOut_Screen)
    {
        BindToAnimationFinished(Anim_FadeOut_Screen, FadeOutFinishedDelegate);
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