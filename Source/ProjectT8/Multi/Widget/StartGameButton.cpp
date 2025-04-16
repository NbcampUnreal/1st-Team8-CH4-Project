#include "Multi/Widget/StartGameButton.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "Components/Button.h"
#include "GameFramework/Common/T8GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UStartGameButton::NativeConstruct()
{
    Super::NativeConstruct();
    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &UStartGameButton::OnStartGameButtonClicked);
    }
}

void UStartGameButton::OnStartGameButtonClicked()
{
    UWorld* World = GetWorld();
    if (World)
    {
        // 로비 상태의 정보를 GameInstance에 저장
        ALobbyGameState* LobbyState = World->GetGameState<ALobbyGameState>();
        UT8GameInstance* GameInstance = Cast<UT8GameInstance>(UGameplayStatics::GetGameInstance(World));

        if (LobbyState && GameInstance)
        {
            // 로비 정보를 GameInstance에 저장
            GameInstance->SaveLobbyGameState(LobbyState);

            UE_LOG(LogTemp, Log, TEXT("ServerTravel: Saved lobby data and attempting to travel to Gameplay Map..."));
        }

        // bUseSeamlessTravel이 GameMode에서 true로 설정되어 있다고 가정
        World->ServerTravel("BattleLevel");
    }
}