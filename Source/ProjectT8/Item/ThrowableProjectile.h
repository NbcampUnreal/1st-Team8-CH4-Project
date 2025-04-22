#pragma once

#include "CoreMinimal.h"
#include "Item/Projectile.h"
#include "ThrowableProjectile.generated.h"


UCLASS()
class PROJECTT8_API AThrowableProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	AThrowableProjectile();

protected:
	virtual void BeginPlay() override;

	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual TSubclassOf<class UGameplayEffect> GetEffectToApply() const;
};
