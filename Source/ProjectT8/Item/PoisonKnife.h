#pragma once
#include "CoreMinimal.h"
#include "Item/Throwable.h"
#include "PoisonKnife.generated.h"

UCLASS()
class PROJECTT8_API APoisonKnife : public AThrowable
{
	GENERATED_BODY()

public:
    APoisonKnife();

protected:
    UPROPERTY(EditAnywhere)
    float FOVReduction;

    UPROPERTY(EditAnywhere)
    float Duration = 5.0f;

};
