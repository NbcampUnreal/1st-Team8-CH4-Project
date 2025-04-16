#include "UI/PlayerStatusIcon.h"
#include "Components/Border.h"
#include "Components/Image.h"

void UPlayerStatusIcon::SetStatus(bool bIsAlive, FLinearColor TeamColor, bool bIsAI)
{
    if (Border_TeamOutline)
        Border_TeamOutline->SetBrushColor(TeamColor);

    if (Image_PlayerIcon)
    {
        // 텍스처 분기
        UTexture2D* SelectedTexture = bIsAI ? AITexture : PlayerTexture;

        if (SelectedTexture)
        {
            FSlateBrush NewBrush;
            NewBrush.SetResourceObject(SelectedTexture);
            Image_PlayerIcon->SetBrush(NewBrush);
        }

        // 생존 여부에 따라 투명도 변경
        Image_PlayerIcon->SetOpacity(bIsAlive ? 1.0f : 0.4f);
    }
}