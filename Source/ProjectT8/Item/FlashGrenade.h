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
    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> BlindEffect;
    TSubclassOf<UGameplayEffect> GetEffectToApply() const override { return BlindEffect; }

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    float BlindRadius = 600.f;
};
