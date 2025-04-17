#include "UI/Widgets/Base/ScreenBaseWidget.h"

#include "Global/Managers/UIManager.h"

void UScreenBaseWidget::OnScreenActivated_Implementation()
{
    // FadeIn 애니메이션 재생
    if (Anim_FadeIn_Screen)
    {
        PlayAnimation(Anim_FadeIn_Screen);
    }
}

void UScreenBaseWidget::OnScreenDeactivated_Implementation()
{
    // FadeOut 애니메이션 Delegate 연결 후, 재생
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