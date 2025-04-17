#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomGameState.h"
#include "ResultGameState.generated.h"


UCLASS()
class PROJECTT8_API AResultGameState : public ACustomGameState
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

private:
	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerState*> WinningPlayerStates;

public:
	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetWinningPlayerStates(const TArray<APlayerState*>& NewWinningPlayerStates);
	
};
