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
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> BlindEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    float FlashRadius = 600.f;
};
