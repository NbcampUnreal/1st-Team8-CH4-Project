#pragma once

#include "CoreMinimal.h"
#include "Item/ThrowableProjectile.h"
#include "PoisonProjectile.generated.h"

UCLASS()
class PROJECTT8_API APoisonProjectile : public AThrowableProjectile
{
	GENERATED_BODY()
public:
	APoisonProjectile();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<class UGameplayEffect> PoisonEffect;

	virtual TSubclassOf<UGameplayEffect> GetEffectToApply() const override { return PoisonEffect; }
};