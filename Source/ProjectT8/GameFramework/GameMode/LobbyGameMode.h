

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT8_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
    ALobbyGameMode();

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void StartGame();

protected:
    // �ʿ��� ���, ��ȯ�� ��ġ ���� �̸��� ����
    UPROPERTY(EditDefaultsOnly, Category = "Lobby")
    FName MatchMapName;
};