#include "Item/CloneGenerator.h"

ACloneGenerator::ACloneGenerator()
{
	ItemName = "CloneGenerator";
}

void ACloneGenerator::Use(ACharacterBase* Player)
{
	//ĳ���Ϳ� Ŭ�н��� ���� �߰��� ...�����ǳ�..?
	// player -> func(���ӽð�)
	Destroy();
}