#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameState/LobbyGameState.h"
#include "NavigationSystem.h"
#include "T8GameMode.generated.h"

UCLASS()
class PROJECTT8_API AT8GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AT8GameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 플레이어 죽음 처리
	UFUNCTION(BlueprintNativeEvent, Category = "Game")
	void NotifyPlayerDeath(ACharacter* DeadCharacter);
	virtual void NotifyPlayerDeath_Implementation(ACharacter* DeadCharacter);
	UFUNCTION(BlueprintCallable, Category = "Game")
	bool CheckGameEnd();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SpawnAIFromSlot(int32 SlotIndex);
	void SpawnAIFromSlotData(const FSlotInfo& Slot);

protected:
	void SpawnAllAIFromLobbySlots();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AT8AICharacter> AIClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AAIController> AIControllerClass;
};
