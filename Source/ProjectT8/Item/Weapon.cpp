#include "Weapon.h"
#include "Player/TestCharacter.h"

AWeapon::AWeapon()
{
    ItemType = EItemType::Weapon;
}

void AWeapon::Use(ATestCharacter* Player)
{
    Attack(); // �⺻������ ���� ȣ�� 
}

void AWeapon::Attack()
{
    // �⺻ ���� ����, �ڽ� Ŭ�������� �� ���⺰�� �������̵�
}