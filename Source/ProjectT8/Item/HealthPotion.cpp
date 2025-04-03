#include "Item/HealthPotion.h"

AHealthPotion::AHealthPotion()
{
	ItemName = "HealthPotion";
}

void AHealthPotion::Use(ATestCharacter* Player)
{
	//캐릭터에 HP+ 로직 추가必
	// player -> func(체력 회복량)
	Destroy();
}