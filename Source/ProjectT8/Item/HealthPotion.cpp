#include "Item/HealthPotion.h"

AHealthPotion::AHealthPotion()
{
	ItemName = "HealthPotion";
}

void AHealthPotion::Use(ATestCharacter* Player)
{
	//ĳ���Ϳ� HP+ ���� �߰���
	// player -> func(ü�� ȸ����)
	Destroy();
}