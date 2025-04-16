#include "Weapon.h"
#include "Player/CharacterBase.h"

AWeapon::AWeapon()
{
    ItemType = EItemType::Weapon;
}

FRotator AWeapon::GetWeaponTypeRotation() const
{
    switch (WeaponType)
    {
    case EWeaponType::Gun:
        return GunRotation;
    case EWeaponType::Melee:
        return MeleeRotation;
    default:
        return FRotator::ZeroRotator;
    }
}

void AWeapon::Use(ACharacterBase* Player)
{
    // 기존 클래스에서 구현
} 