#pragma once
#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Throwable.generated.h"


UCLASS()
class PROJECTT8_API AThrowable : public ABaseItem
{
	GENERATED_BODY()

public:
	AThrowable();

	virtual void Use(ACharacterBase* Player) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float ThrowForce = 1000.f;
};