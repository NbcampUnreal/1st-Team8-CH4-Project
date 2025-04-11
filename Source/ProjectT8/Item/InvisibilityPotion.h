#pragma once
#include "CoreMinimal.h"
#include "Item/Consumable.h"
#include "InvisibilityPotion.generated.h"

UCLASS()
class PROJECTT8_API AInvisibilityPotion : public AConsumable
{
	GENERATED_BODY()

public:
    AInvisibilityPotion();
    virtual void Use(class ACharacterBase* Player) override;

protected:
    UPROPERTY(EditAnywhere)
    float Duration = 10.0f;
};