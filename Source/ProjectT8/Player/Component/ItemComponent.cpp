#include "ItemComponent.h"
#include "Player/CharacterBase.h"
#include "CombatComponent.h"
#include "Item/BaseItem.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UItemComponent::Init(ACharacterBase* InOwner)
{
	OwnerCharacter = InOwner;
}

void UItemComponent::TryPickUpItem(ABaseItem* NewItem)
{
	if (!OwnerCharacter || !NewItem) return;

	if (EquippedItem)
	{
		DropItemToWorld();
	}

	EquippedItem = NewItem;
	EquippedItem->SetOwner(OwnerCharacter);

	if (OwnerCharacter && OwnerCharacter->GetCombatComponent())
	{
		OwnerCharacter->GetCombatComponent()->CurrentDamageEffect = EquippedItem->GetAssociatedGameplayEffect();
	}

	if (OwnerCharacter->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("HasAuthority"));
		Multicast_AttachItem(NewItem);
	}
}

void UItemComponent::UseEquippedItem()
{
	if (EquippedItem)
	{
		EquippedItem->Use(OwnerCharacter);
		UE_LOG(LogTemp, Warning, TEXT("사용한 아이템: %s"), *GetNameSafe(EquippedItem));
	}
}

void UItemComponent::DropItemToWorld()
{
	if (!EquippedItem) return;

	EquippedItem->SetOwner(nullptr);
	EquippedItem->SetActorEnableCollision(true);
	EquippedItem->SetActorHiddenInGame(false);
	EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(EquippedItem->GetRootComponent()))
	{
		Prim->SetSimulatePhysics(true);
	}

	EquippedItem->SetActorLocation(OwnerCharacter->GetActorLocation() + DropOffset);
	
	EquippedItem = nullptr;
}

void UItemComponent::Multicast_AttachItem_Implementation(ABaseItem* Item)
{
	if (!Item || !OwnerCharacter) return;

	Item->SetActorHiddenInGame(false);
	Item->SetActorEnableCollision(false);

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		Prim->SetSimulatePhysics(false);
		Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	Item->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
	UE_LOG(LogTemp, Error, TEXT("check ehck"));
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
