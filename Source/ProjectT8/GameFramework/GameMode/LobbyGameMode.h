

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
    // 필요한 경우, 전환될 매치 맵의 이름을 설정
    UPROPERTY(EditDefaultsOnly, Category = "Lobby")
    FName MatchMapName;
};