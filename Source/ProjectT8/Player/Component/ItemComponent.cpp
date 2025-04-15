#include "ItemComponent.h"
#include "GameFramework/Character.h"
#include "CombatComponent.h"
#include "Item/BaseItem.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CharacterBase.h"


UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UItemComponent::Init(ACharacter* InOwner)
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

	if (OwnerCharacter && Cast<ACharacterBase>(OwnerCharacter)->GetCombatComponent())
	{
		Cast<ACharacterBase>(OwnerCharacter)->GetCombatComponent()->CurrentDamageEffect = EquippedItem->GetAssociatedGameplayEffect();
	}

	if (OwnerCharacter->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("HasAuthority"));
		Multicast_AttachItem(NewItem);
		if (ACharacterBase* Character = Cast<ACharacterBase>(OwnerCharacter))
		{
			Character->OnWeaponEquipped();
		}
	}
}

void UItemComponent::UseEquippedItem()
{
	if (EquippedItem)
	{
		EquippedItem->Use(Cast<ACharacterBase>(OwnerCharacter));
		UE_LOG(LogTemp, Warning, TEXT("사용한 아이템: %s"), *GetNameSafe(EquippedItem));
	}
}

void UItemComponent::DropItemToWorld()
{
	if (!EquippedItem) return;

	if (ACharacterBase* Character = Cast<ACharacterBase>(OwnerCharacter))
	{
		Character->OnWeaponUnequipped();
	}

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
	Item->SetActorRelativeTransform(Item->AttachOffset);
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemComponent, EquippedItem);
}
