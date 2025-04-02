#pragma once
#include "CoreMinimal.h"
#include "Item/Projectile.h"
#include "ElectricProjectile.generated.h"

UCLASS()
class PROJECTT8_API AElectricProjectile : public AProjectile
{
	GENERATED_BODY()
public:
    AElectricProjectile();
    virtual void ApplyEffect(ATestCharacter* Target) override;

protected:
    UPROPERTY(EditAnywhere)
    float StunDuration = 3.0f;
};