#pragma once
#include "CoreMinimal.h"
#include "Item/Consumable.h"
#include "CloneGenerator.generated.h"

UCLASS()
class PROJECTT8_API ACloneGenerator : public AConsumable
{
	GENERATED_BODY()
public:
    ACloneGenerator();
    virtual void Use(class ATestCharacter* Player) override;

protected:
    UPROPERTY(EditAnywhere)
    float CloneDuration = 20.0f;	
};