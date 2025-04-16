#include "UI/TeamStatusBar.h"

void UTeamStatusBar::InitPlayerIcons(const TArray<FPlayerStatusUIData>& PlayerList)
{   
    HorizontalBox_Players->ClearChildren();

    for (const FPlayerStatusUIData& Data : PlayerList)
    {
        UPlayerStatusIcon* Icon = CreateWidget<UPlayerStatusIcon>(this, PlayerStatusIconClass);
        Icon->SetStatus(Data.bIsAlive, Data.TeamColor);
        HorizontalBox_Players->AddChildToHorizontalBox(Icon);
    }
}