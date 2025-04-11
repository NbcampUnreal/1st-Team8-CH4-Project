#include "Item/CloneGenerator.h"

ACloneGenerator::ACloneGenerator()
{
	ItemName = "CloneGenerator";
}

void ACloneGenerator::Use(ACharacterBase* Player)
{
	//캐릭터에 클론스폰 로직 추가必 ...구현되나..?
	// player -> func(지속시간)
	Destroy();
}