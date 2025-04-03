#pragma once
#include "CoreMinimal.h"
#include "Item/Consumable.h"
#include "HealthPotion.generated.h"

UCLASS()
class PROJECTT8_API AHealthPotion : public AConsumable
{
	GENERATED_BODY()

public:
    AHealthPotion();
    virtual void Use(class ATestCharacter* Player) override;

protected:
    UPROPERTY(EditAnywhere)
    float HealthRestore = 50.0f;
};