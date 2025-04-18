#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomPlayerController.h"
#include "LoadingPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PROJECTT8_API ALoadingPlayerController : public ACustomPlayerController
{
	GENERATED_BODY()
	
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* LoadingMappingContext;

    // Input Actions
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* NextPhaseAction;

private:
    void HandleNextPhaseInput();
};
