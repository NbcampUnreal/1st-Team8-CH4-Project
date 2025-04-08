#include "Weapon.h"
#include "Player/CharacterBase.h"

AWeapon::AWeapon()
{
    ItemType = EItemType::Weapon;
}

void AWeapon::Use(ACharacterBase* Player)
{
    // 하위 클래스에서 구현
} 