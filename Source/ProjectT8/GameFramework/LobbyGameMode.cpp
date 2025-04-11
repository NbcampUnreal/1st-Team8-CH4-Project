#include "GameFramework/LobbyGameMode.h"
#include "LobbyGameState.h"
#include "Kismet/GameplayStatics.h"

ALobbyGameMode::ALobbyGameMode()
{
    // ����: ��ġ �� �̸� ���� (������Ʈ�� �°� ����)
    MatchMapName = FName(TEXT("MatchMap"));
}

void ALobbyGameMode::StartGame()
{
    // ���������� �����
    ALobbyGameState* LobbyState = GetGameState<ALobbyGameState>();
    if (LobbyState)
    {
        // �κ��� ������(�� ����, ���õ� ��, ���� ����)�� �ʿ信 ���� ó���ϰų� �����մϴ�.
        // ���� ���, ��� �÷��̾�� �ʿ��� ������ �����ϱ� ���� PlayerState�� ���� ������ �� �ֽ��ϴ�.
    }

    // Seamless Travel�� ����Ͽ� ���ο� ������ �̵� (PlayerState�� �����˴ϴ�)
    // ���� �÷��̾���� ���ο� �� ����, ���� ��ġ ���� �� GameState���� ����ؾ� �Ѵٸ�,
    // �ش� �����ʹ� �̹� LobbyGameState �Ǵ� PlayerState�� ����Ǿ� �����Ƿ�, �� GameState���� ���� �� �ֽ��ϴ�.
    UGameplayStatics::OpenLevel(GetWorld(), MatchMapName, true, "listen");
}