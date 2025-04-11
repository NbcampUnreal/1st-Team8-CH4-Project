#include "GameFramework/GameMode/LobbyGameMode.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "Kismet/GameplayStatics.h"

ALobbyGameMode::ALobbyGameMode()
{
    // 예시: 매치 맵 이름 설정 (프로젝트에 맞게 변경)
    MatchMapName = FName(TEXT("MatchMap"));
}

void ALobbyGameMode::StartGame()
{
    // 서버에서만 실행됨
    ALobbyGameState* LobbyState = GetGameState<ALobbyGameState>();
    if (LobbyState)
    {
        // 로비의 데이터(팀 구성, 선택된 맵, 슬롯 정보)를 필요에 따라 처리하거나 저장합니다.
        // 예를 들어, 모든 플레이어에게 필요한 정보를 전달하기 위해 PlayerState에 값을 저장할 수 있습니다.
    }

    // Seamless Travel을 사용하여 새로운 맵으로 이동 (PlayerState는 유지됩니다)
    // 만약 플레이어들이 새로운 팀 구성, 스폰 위치 등을 새 GameState에서 사용해야 한다면,
    // 해당 데이터는 이미 LobbyGameState 또는 PlayerState에 저장되어 있으므로, 새 GameState에서 읽을 수 있습니다.
    UGameplayStatics::OpenLevel(GetWorld(), MatchMapName, true, "listen");
}