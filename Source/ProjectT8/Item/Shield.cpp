#include "Item/Shield.h"

AShield::AShield()
{
	ItemName = "Shield";
}

void AShield::Use(ATestCharacter* Player)
{
	//ĳ���Ϳ� �ǵ� ���� �߰���
	// player -> func(�����, ���ӽð�)
    Destroy();
}