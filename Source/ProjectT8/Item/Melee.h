#pragma once
#include "CoreMinimal.h"
#include "Item/Weapon.h"
#include "Melee.generated.h"

UCLASS()
class PROJECTT8_API AMelee : public AWeapon
{
    GENERATED_BODY()
public:
    AMelee();
    virtual void Attack() override;

protected:
    UPROPERTY(EditAnywhere)
    float Damage = 10.0f;

    UPROPERTY(EditAnywhere)
    float AttackRange = 100.0f;
};