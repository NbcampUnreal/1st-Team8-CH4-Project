#include "Item/SpeedBoostPotion.h"

ASpeedBoostPotion::ASpeedBoostPotion()
{
	ItemName = "SpeedBoostPotion";
}

void ASpeedBoostPotion::Use(ACharacterBase* Player)
{
	//ĳ���Ϳ� ���ǵ�+ ���� �߰���
	// player -> func(���ӽð�, ��������)
	Destroy();
}