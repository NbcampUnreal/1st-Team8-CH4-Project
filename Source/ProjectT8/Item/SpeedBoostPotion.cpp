#include "Item/SpeedBoostPotion.h"

ASpeedBoostPotion::ASpeedBoostPotion()
{
	ItemName = "SpeedBoostPotion";
}

void ASpeedBoostPotion::Use(ATestCharacter* Player)
{
	//ĳ���Ϳ� ���ǵ�+ ���� �߰���
	// player -> func(���ӽð�, ��������)
	Destroy();
}