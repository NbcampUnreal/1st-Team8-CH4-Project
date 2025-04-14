#pragma once
#include "CoreMinimal.h"
#include "Item/Projectile.h"
#include "FireProjectile.generated.h"

class UGameplayEffect;

UCLASS()
class PROJECTT8_API AFireProjectile : public AProjectile
{
	GENERATED_BODY()
public:
    AFireProjectile();

protected:
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> BurnEffect;
};