#pragma once
#include "CoreMinimal.h"
#include "Item/Projectile.h"
#include "IceProjectile.generated.h"

UCLASS()
class PROJECTT8_API AIceProjectile : public AProjectile
{
	GENERATED_BODY()
public:
    AIceProjectile();
    virtual void ApplyEffect(ATestCharacter* Target) override;

protected:
    UPROPERTY(EditAnywhere)
    float FreezeDuration = 3.0f;

    UPROPERTY(EditAnywhere)
    float SlideSpeed = 200.0f;
};
