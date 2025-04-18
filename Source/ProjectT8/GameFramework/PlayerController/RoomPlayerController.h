#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Common/CustomPlayerController.h"
#include "RoomPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PROJECTT8_API ARoomPlayerController : public ACustomPlayerController
{
	GENERATED_BODY()
	
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* RoomMappingContext;

    // Input Actions
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* RoomNextPhaseAction;

private:
    void HandleNextPhaseInput();
};
