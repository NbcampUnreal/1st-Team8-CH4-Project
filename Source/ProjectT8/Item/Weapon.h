#pragma once
#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Gun,
    Melee
};

UCLASS()
class PROJECTT8_API AWeapon : public ABaseItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* IdleMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* AttackMontage;
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType WeaponType;

public:
    AWeapon();

    virtual void Use(class ACharacterBase* Player) override;
};