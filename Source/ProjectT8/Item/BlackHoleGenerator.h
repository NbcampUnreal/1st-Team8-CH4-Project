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

protected:
    UPROPERTY(EditAnywhere)
    float RangeRadius = 500.0f;
};
