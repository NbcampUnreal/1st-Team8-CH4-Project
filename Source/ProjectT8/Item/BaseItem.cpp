#include "BaseItem.h"
#include "Player/CharacterBase.h"
#include "Player/Component/ItemComponent.h"


ABaseItem::ABaseItem()
{
	bReplicates = true;
	SetReplicateMovement(true);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	ItemMesh->SetCollisionObjectType(ECC_PhysicsBody);
	ItemMesh->SetEnableGravity(true);
	SetRootComponent(ItemMesh);

	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->SetupAttachment(ItemMesh);
	InteractSphere->SetSphereRadius(40.0f);
	InteractSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractSphere->SetGenerateOverlapEvents(true);

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