#pragma once
#include "CoreMinimal.h"
#include "Item/Projectile.h"
#include "ElectricProjectile.generated.h"

class UGameplayEffect;

UCLASS()
class PROJECTT8_API AElectricProjectile : public AProjectile
{
	GENERATED_BODY()
public:
    AElectricProjectile();
protected:
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> ShockEffect;
};