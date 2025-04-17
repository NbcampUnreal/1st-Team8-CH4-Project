#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "T8GameMode.generated.h"

UCLASS()
class PROJECTT8_API AT8GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AT8GameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 플레이어 죽음 처리
	UFUNCTION(BlueprintNativeEvent, Category = "Game")
	void NotifyPlayerDeath(ACharacter* DeadCharacter);
	virtual void NotifyPlayerDeath_Implementation(ACharacter* DeadCharacter);
};
