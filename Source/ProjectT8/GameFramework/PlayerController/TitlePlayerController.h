#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomPlayerController.h"
#include "TitlePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PROJECTT8_API ATitlePlayerController : public ACustomPlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* TitleMappingContext;

    // Input Actions
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* StartGameAction;

private:
    void HandleStartInput();
};
