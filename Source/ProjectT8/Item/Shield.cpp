#include "Item/Shield.h"

AShield::AShield()
{
	ItemName = "Shield";
}

void AShield::Use(ACharacterBase* Player)
{
	//ĳ���Ϳ� �ǵ� ���� �߰���
	// player -> func(�����, ���ӽð�)
    Destroy();
}