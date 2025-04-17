#include "UI/Widgets/Screen/PlayingScreenWidget.h"
#include "UI/TeamStatusBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "GameFramework/Common/T8GameInstance.h"
#include "GameFramework/GameState/T8GameState.h"
#include "GameFramework/Character.h"
#include "Player/CharacterBase.h"
#include "Kismet/GameplayStatics.h"


void UPlayingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AT8GameState* GS = Cast<AT8GameState>(UGameplayStatics::GetGameState(GetWorld()));

	if (!GS) return;

	TArray<FPlayerStatusUIData> PUIList = GS->PlayerUIList;
	
	for (const FPlayerStatusUIData& Data : PUIList)
	{
		PlayerStatusMap.Add(Data.DisplayName, Data);
	}

	if (TeamStatusBar)
	{
		TeamStatusBar->InitPlayerIcons(PUIList);
	}
	
	if (UT8GameInstance* GI = Cast<UT8GameInstance>(GetGameInstance()))
	{
		if (TeamModeText)
		{
			switch (GI->SavedTeamSetup)
			{
			case ETeamSetup::FreeForAll:
				TeamModeText->SetText(FText::FromString("FreeForAll"));
				break;
			case ETeamSetup::TwoTeams:
				TeamModeText->SetText(FText::FromString("TwoTeams"));
				break;
			case ETeamSetup::FourTeams:
				TeamModeText->SetText(FText::FromString("FourTeams"));
				break;
			default:
				TeamModeText->SetText(FText::FromString("FreeForAll"));
				break;
			}
		}
	}
}

void UPlayingScreenWidget::OnCharacterDeath(ACharacter* DeadCharacter)
{
	if (!DeadCharacter) return;

	ACharacterBase* BaseCharacter = Cast<ACharacterBase>(DeadCharacter);
	if (!BaseCharacter) return;

	UpdatePlayerStatus(BaseCharacter->PlayerDisplayName, false);
}

void UPlayingScreenWidget::UpdatePlayerStatus(const FString& PlayerName, bool bIsAlive)
{
	// 플레이어 상태 데이터 업데이트
	if (FPlayerStatusUIData* PlayerData = PlayerStatusMap.Find(PlayerName))
	{
		PlayerData->bIsAlive = bIsAlive;

		// TeamStatusBar 업데이트
		if (TeamStatusBar)
		{
			TArray<FPlayerStatusUIData> UpdatedList;
			PlayerStatusMap.GenerateValueArray(UpdatedList);
			TeamStatusBar->InitPlayerIcons(UpdatedList);
		}
	}
}
