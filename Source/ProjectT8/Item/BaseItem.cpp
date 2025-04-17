#include "BaseItem.h"
#include "Player/CharacterBase.h"
#include "Player/Component/ItemComponent.h"


ABaseItem::ABaseItem()
{
	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	ItemMesh->SetCollisionObjectType(ECC_PhysicsBody);

	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->SetupAttachment(ItemMesh);
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
	if (!HasAuthority()) return;
	
	if (Player && !GetOwner())
	{
		if (UItemComponent* ItemComp = Player->ItemComponent)
		{
			ItemComp->TryPickUpItem(this);
		}
	}
}

void ABaseItem::Use(ACharacterBase* Player)
{
}

void ABaseItem::Server_Use_Implementation(ACharacterBase* Player)
{
	Use(Player);
}