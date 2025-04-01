#pragma once
#include "CoreMinimal.h"
#include "Item/Weapon.h"
#include "Gun.generated.h"

UCLASS()
class PROJECTT8_API AGun : public AWeapon
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentAmmo = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxAmmo = 5;

    UPROPERTY(EditAnywhere)
    TSubclassOf<class AProjectile> ProjectileClass;
public:
    AGun();

    virtual void Attack() override;
};
