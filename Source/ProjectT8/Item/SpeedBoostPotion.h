#pragma once
#include "CoreMinimal.h"
#include "Item/Consumable.h"
#include "SpeedBoostPotion.generated.h"

UCLASS()
class PROJECTT8_API ASpeedBoostPotion : public AConsumable
{
	GENERATED_BODY()

public:
	ASpeedBoostPotion();
	virtual void Use(class ACharacterBase* Player) override;

protected:
	UPROPERTY(EditAnywhere)
	float Duration = 5.0f;
	float SpeedIncreaseRatio = 2.0f;
};