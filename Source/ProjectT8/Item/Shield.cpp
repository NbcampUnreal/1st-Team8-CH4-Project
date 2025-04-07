#include "Item/Shield.h"

AShield::AShield()
{
	ItemName = "Shield";
}

void AShield::Use(ACharacterBase* Player)
{
	//캐릭터에 실드 로직 추가必
	// player -> func(쉴드양, 지속시간)
    Destroy();
}