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
     UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> PoisonEffect;

	TSubclassOf<UGameplayEffect> GetEffectToApply() const override { return PoisonEffect;}

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    float PoisonRadius = 300.f;
};
