#include "UI/Widgets/Screen/PlayingScreenWidget.h"
#include "UI/TeamStatusBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "GameFramework/Common/T8GameInstance.h"



void UPlayingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UT8GameInstance* GI = Cast<UT8GameInstance>(GetGameInstance());
	if (!GI) return;

	TArray<FPlayerStatusUIData> PlayerUIList;
	
	for (const FSlotInfo& EachSlot : GI->SavedLobbySlots)
	{
		FPlayerStatusUIData UIData;

		/*if (GEngine)
		{
			FString LogMessage = FString::Printf(
				TEXT("EachSlot DisplayName: %s, bIsAI: %s, TeamNumber: %d"),
				*EachSlot.DisplayName,
				EachSlot.bIsAI ? TEXT("True") : TEXT("False"),
				EachSlot.TeamNumber
			);

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, LogMessage);
		}*/

		UIData.DisplayName = EachSlot.DisplayName;
		UIData.bIsAlive = true;
		UIData.bIsAI = EachSlot.bIsAI;

		switch (EachSlot.TeamNumber)
		{
			case 0 : UIData.TeamColor = FLinearColor::Red; break;
			case 1: UIData.TeamColor = FLinearColor::Blue; break;
			case 2: UIData.TeamColor = FLinearColor::Green; break;
			case 3: UIData.TeamColor = FLinearColor::Yellow; break;
			default: UIData.TeamColor = FLinearColor::Gray; break;
		}

		PlayerUIList.Add(UIData);
	}

	if (TeamStatusBar)
	{
		TeamStatusBar->InitPlayerIcons(PlayerUIList);
	}

	if (TeamModeText)
	{
		switch (GI->SavedTeamSetup)	
		{
			case ETeamSetup::FreeForAll:
				TeamModeText->SetText(FText::FromString("FreeForAll"));
			case ETeamSetup::TwoTeams:
				TeamModeText->SetText(FText::FromString("TwoTeams"));
			case ETeamSetup::FourTeams:
				TeamModeText->SetText(FText::FromString("FourTeams"));
			default:
				TeamModeText->SetText(FText::FromString("FreeForAll"));
				break;
		}
	}
}
