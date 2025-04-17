#include "UI/Widgets/Screen/TitleScreenWidget.h"


void UTitleScreenWidget::OnScreenActivated_Implementation()
{
    Super::OnScreenActivated_Implementation();

    if (Anim_TextBlink_Title)
    {
        PlayAnimation(Anim_TextBlink_Title, 0.f, 0);
    }
}