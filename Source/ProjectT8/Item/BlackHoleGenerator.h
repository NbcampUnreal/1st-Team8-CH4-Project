#pragma once
#include "CoreMinimal.h"
#include "Item/Throwable.h"
#include "BlackHoleGenerator.generated.h"

UCLASS()
class PROJECTT8_API ABlackHoleGenerator : public AThrowable
{
	GENERATED_BODY()

public:
    ABlackHoleGenerator();
    virtual void ApplyEffect(ACharacterBase* Target) override;
};
