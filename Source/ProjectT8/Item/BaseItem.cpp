#include "BaseItem.h"
#include "Player/CharacterBase.h"


ABaseItem::ABaseItem()
{
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;
	ItemName = "DefaultItem";
	PrimaryActorTick.bCanEverTick = false;

}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseItem::Interact_Implementation(ACharacterBase* Player)
{
	/*if (Player)
	{
		Player->PickupItem(this);
	}*/
}

void ABaseItem::Use(ACharacterBase* Player)
{
	// 기본 사용 로직, 파생 클래스에서 오버라이드
}