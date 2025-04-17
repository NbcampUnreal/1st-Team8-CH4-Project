#include "UI/TeamStatusBar.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerStatusIcon.h"

void UTeamStatusBar::InitPlayerIcons(const TArray<FPlayerStatusUIData>& PlayerList)
{   
    HorizontalBox_Players->ClearChildren();

    for (const FPlayerStatusUIData& Data : PlayerList)
    {
        UPlayerStatusIcon* Icon = CreateWidget<UPlayerStatusIcon>(this, PlayerStatusIconClass);
        Icon->SetStatus(Data.bIsAlive, Data.TeamColor, Data.bIsAI);
        HorizontalBox_Players->AddChildToHorizontalBox(Icon);
    }
}