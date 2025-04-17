#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "T8PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FCharacterAppearanceData;
UCLASS()
class PROJECTT8_API AT8PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    UFUNCTION(Server, Reliable)
    void Server_SendMyAppearance(const FCharacterAppearanceData& MyData);

    UFUNCTION(Client, Reliable)
    void Client_TriggerSendAppearance();

protected:
	virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* PlayingMappingContext;

    // Input Actions
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* NextPhaseAction;

private:
	void HandleNextPhaseInput();
};
