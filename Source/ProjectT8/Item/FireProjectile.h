#pragma once
#include "CoreMinimal.h"
#include "Item/Projectile.h"
#include "FireProjectile.generated.h"

UCLASS()
class PROJECTT8_API AFireProjectile : public AProjectile
{
	GENERATED_BODY()
public:
    AFireProjectile();
    virtual void ApplyEffect(ATestCharacter* Target) override;

protected:
    UPROPERTY(EditAnywhere)
    float BurnDuration = 10.0f;

    UPROPERTY(EditAnywhere)
    float DamagePerSecond = 5.0f;
};