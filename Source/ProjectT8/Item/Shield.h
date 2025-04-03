#pragma once
#include "CoreMinimal.h"
#include "Item/Consumable.h"
#include "Shield.generated.h"

UCLASS()
class PROJECTT8_API AShield : public AConsumable
{
	GENERATED_BODY()

public:
    AShield();
    virtual void Use(class ATestCharacter* Player) override;

protected:
    UPROPERTY(EditAnywhere)
    float ShieldAmount = 30.0f;

    UPROPERTY(EditAnywhere)
    float Duration = 15.0f;
};