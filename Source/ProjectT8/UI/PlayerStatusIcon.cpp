#include "UI/PlayerStatusIcon.h"

void UPlayerStatusIcon::SetStatus(bool bIsAlive, FLinearColor TeamColor)
{
    if (Border_TeamOutline)
        Border_TeamOutline->SetBrushColor(TeamColor);

    if (Image_PlayerIcon)
        Image_PlayerIcon->SetOpacity(bIsAlive ? 1.0f : 0.4f); // 사망 시 흐리게
}