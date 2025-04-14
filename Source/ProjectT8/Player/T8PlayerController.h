#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/Customize/FCharacterAppearanceData.h"
#include "T8PlayerController.generated.h"

UCLASS()
class PROJECTT8_API AT8PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerSetAppearanceData(const FCharacterAppearanceData& InData);
};
