#include "ItemComponent.h"
#include "GameFramework/Character.h"
#include "CombatComponent.h"
#include "Item/BaseItem.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CharacterBase.h"
#include "AI/T8AICharacter.h"
#include "Item/Weapon.h"
#include "Item/Throwable.h"


UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	EquippedItem = nullptr;
}

void UItemComponent::Init(ACharacter* InOwner)
{
	OwnerCharacter = InOwner;
}

void UItemComponent::TryPickUpItem(ABaseItem* NewItem)
{
	if (!OwnerCharacter || !NewItem) return;

	// 아이템의 주인이 있을 경우 종료
	if (NewItem->GetOwner()) return;

	if (EquippedItem)
	{
		DropItemToWorld();
	}
	EquippedItem = NewItem;
	EquippedItem->SetOwner(OwnerCharacter);
	UE_LOG(LogTemp, Warning, TEXT("[TryPickUpItem] Equipped: %s"), *GetNameSafe(NewItem));

	if (ACharacterBase* PlayerCharacter = Cast<ACharacterBase>(OwnerCharacter))
	{
		if (UCombatComponent* Combat = PlayerCharacter->GetCombatComponent())
		{
			Combat->CurrentDamageEffect = EquippedItem->GetAssociatedGameplayEffect();
		}
	}
	else if (AT8AICharacter* AICharacter = Cast<AT8AICharacter>(OwnerCharacter))
	{
		if (UCombatComponent* Combat = AICharacter->GetCombatComponent())
		{
			Combat->CurrentDamageEffect = EquippedItem->GetAssociatedGameplayEffect();
		}
	}

	if (OwnerCharacter->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("HasAuthority : AttachItem - %s"), *GetNameSafe(NewItem));
		Multicast_AttachItem(NewItem);
	}
}


void UItemComponent::UseEquippedItem()
{
	if (!EquippedItem || !OwnerCharacter) return;
	if (OwnerCharacter->HasAuthority())
	{
		EquippedItem->Use(Cast<ACharacterBase>(OwnerCharacter));
		UE_LOG(LogTemp, Warning, TEXT("사용한 아이템: %s"), *GetNameSafe(EquippedItem));
	}
	else
	{
		Server_UseEquippedItem();
	}
}

void UItemComponent::Server_UseEquippedItem_Implementation()
{
	UseEquippedItem();
}

void UItemComponent::DropItemToWorld()
{
	if (!EquippedItem) return;

	EquippedItem->SetOwner(nullptr);
	EquippedItem->SetActorEnableCollision(true);
	EquippedItem->SetActorHiddenInGame(false);
	EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	EquippedItem->SetActorLocation(OwnerCharacter->GetActorLocation() + DropOffset);

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(EquippedItem->GetRootComponent()))
	{
		Prim->SetSimulatePhysics(true);
		Prim->SetEnableGravity(true);
		Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	EquippedItem = nullptr;
}

void UItemComponent::Multicast_AttachItem_Implementation(ABaseItem* Item)
{
	if (!Item || !OwnerCharacter) return;

	Item->SetActorHiddenInGame(false);
	Item->SetActorEnableCollision(false);


	if (Item->GetItemMesh())
	{
		Item->GetItemMesh()->SetVisibility(true);
		Item->GetItemMesh()->SetSimulatePhysics(false);
		Item->GetItemMesh()->SetEnableGravity(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttachItem] WARNING: ItemMesh is nullptr for item: %s"), *GetNameSafe(Item));
	}

	Item->GetItemMesh()->SetWorldLocation(OwnerCharacter->GetActorLocation() + FVector(0, 0, 50));
	Item->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("WeaponSocket"));
		
	// 기본 AttachOffset 적용
	FTransform ItemTransform = Item->AttachOffset;
	
	// 무기인 경우 타입별 회전값 적용
	if (AWeapon* Weapon = Cast<AWeapon>(Item))
	{
		FRotator TypeRotation = Weapon->GetWeaponTypeRotation();
		ItemTransform.SetRotation(FQuat(TypeRotation) * ItemTransform.GetRotation());
	}
	ItemTransform.SetScale3D(Item->GetActorScale3D());
	Item->SetActorRelativeTransform(ItemTransform);
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{ 
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemComponent, EquippedItem);
}