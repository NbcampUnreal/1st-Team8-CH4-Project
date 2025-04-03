#pragma once
#include "CoreMinimal.h"
#include "Item/Throwable.h"
#include "FlashGrenade.generated.h"

UCLASS()
class PROJECTT8_API AFlashGrenade : public AThrowable
{
    GENERATED_BODY()

public:
    AFlashGrenade();

protected:
    UPROPERTY(EditAnywhere)
    float RangeRadius = 500.0f;

    UPROPERTY(EditAnywhere)
    float FlashDuration = 2.0f;
};
