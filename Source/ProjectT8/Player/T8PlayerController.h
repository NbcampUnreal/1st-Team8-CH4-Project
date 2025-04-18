#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomPlayerController.h"
#include "Player/Customize/FCharacterAppearanceData.h"
#include "T8PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PROJECTT8_API AT8PlayerController : public ACustomPlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* PlayingMappingContext;

    // Input Actions
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* NextPhaseAction;

	UFUNCTION(Server, Reliable)
	void ServerSetAppearanceData(const FCharacterAppearanceData& InData);

private:
	void HandleNextPhaseInput();
};
