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

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType WeaponType;

public:
    AWeapon();

    virtual void Use(class ATestCharacter* Player) override;
    virtual void Attack(); // 무기 사용 시 호출
};
