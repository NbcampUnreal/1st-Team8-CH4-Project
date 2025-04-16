#pragma once
#include "CoreMinimal.h"
#include "Item/Weapon.h"
#include "Gun.generated.h"


UCLASS()
class PROJECTT8_API AGun : public AWeapon
{
	GENERATED_BODY()

protected:
    UFUNCTION(Server, Reliable)
    void Server_Use(ACharacterBase* Player);

    UPROPERTY(Replicated)
    int32 CurrentAmmo = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxAmmo = 5;

    UPROPERTY(EditAnywhere)
    TSubclassOf<class AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere)
    UAnimMontage* GunAttackMontage;
public:
    AGun(); 
    virtual void Use(ACharacterBase* Player) override;
};