#include "Item/SpeedBoostPotion.h"

ASpeedBoostPotion::ASpeedBoostPotion()
{
	ItemName = "SpeedBoostPotion";
}

void ASpeedBoostPotion::Use(ATestCharacter* Player)
{
	//캐릭터에 스피드+ 로직 추가必
	// player -> func(지속시간, 증가배율)
	Destroy();
}