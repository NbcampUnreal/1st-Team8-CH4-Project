#include "Weapon.h"
#include "Player/CharacterBase.h"

AWeapon::AWeapon()
{
    ItemType = EItemType::Weapon;
}

void AWeapon::Use(ACharacterBase* Player)
{
    Attack(); // 기본적으로 공격 호출 
}

void AWeapon::Attack()
{
    // 기본 공격 로직, 자식 클래스에서 각 무기별로 오버라이드
}