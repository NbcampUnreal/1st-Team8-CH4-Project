#pragma once

#include "CoreMinimal.h"
#include "Item/ThrowableProjectile.h"
#include "FlashProjectile.generated.h"

UCLASS()
class PROJECTT8_API AFlashProjectile : public AThrowableProjectile
{
	GENERATED_BODY()
public:
	AFlashProjectile();
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<class UGameplayEffect> FlashEffect;

	virtual TSubclassOf<UGameplayEffect> GetEffectToApply() const override { return FlashEffect; }
};