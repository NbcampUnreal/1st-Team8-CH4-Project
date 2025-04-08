#include "BaseItem.h"
#include "Player/CharacterBase.h"


ABaseItem::ABaseItem()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComp);

	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->SetupAttachment(RootComp);
	InteractSphere->SetSphereRadius(200.0f);
	InteractSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ItemName = "DefaultItem";
	PrimaryActorTick.bCanEverTick = false;

}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseItem::Interact_Implementation(ACharacterBase* Player)
{
	if (Player)
	{
		Player->PickupItem(this);
	}
}

void ABaseItem::Use(ACharacterBase* Player)
{
	// 기본 사용 로직, 파생 클래스에서 오버라이드
}