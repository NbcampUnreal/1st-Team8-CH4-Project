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
    UAnimMontage* AttackMontage;

    // 무기 타입별 기본 회전값 반환
    FRotator GetWeaponTypeRotation() const;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType WeaponType;

    // 무기 타입별 기본 회전값 설정
    UPROPERTY(EditDefaultsOnly, Category = "Transform")
    FRotator GunRotation = FRotator(0.0f, 0.0f, 0.0f);  // 총기류 기본 회전값

    UPROPERTY(EditDefaultsOnly, Category = "Transform")
    FRotator MeleeRotation = FRotator(0.0f, 0.0f, 0.0f);  // 둔기류 기본 회전값

public:
    AWeapon();
    virtual void Use(class ACharacterBase* Player) override;

    FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
};