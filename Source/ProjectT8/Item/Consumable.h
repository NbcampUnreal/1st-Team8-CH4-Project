#pragma once
#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Consumable.generated.h"

UCLASS()
class PROJECTT8_API AConsumable : public ABaseItem
{
	GENERATED_BODY()

public:
    AConsumable();
    virtual void Use(ATestCharacter* Player) override;
};