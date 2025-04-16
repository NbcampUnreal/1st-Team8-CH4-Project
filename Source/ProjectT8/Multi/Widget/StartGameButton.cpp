#include "Multi/Widget/StartGameButton.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "Components/Button.h"


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
        // 로비 상태 등을 확인하고 게임 시작 조건을 만족하는지 검사...
        // 예: 모든 플레이어가 준비되었는지, 최소 인원이 충족되었는지 등

        UE_LOG(LogTemp, Log, TEXT("ServerTravel: Attempting to travel to Gameplay Map..."));

        // bUseSeamlessTravel이 GameMode에서 true로 설정되어 있다고 가정

        World->ServerTravel("BattleLevel");
    }
}
