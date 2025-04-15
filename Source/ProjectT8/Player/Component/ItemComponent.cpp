#include "ItemComponent.h"
#include "GameFramework/Character.h"
#include "CombatComponent.h"
#include "Item/BaseItem.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CharacterBase.h"
#include "AI/T8AICharacter.h"
#include "Item/Weapon.h"


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
		UE_LOG(LogTemp, Warning, TEXT("HasAuthority"));
		Multicast_AttachItem(NewItem);
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
	
	// 기본 AttachOffset 적용
	FTransform ItemTransform = Item->AttachOffset;
	
	// 무기인 경우 타입별 회전값 적용
	if (AWeapon* Weapon = Cast<AWeapon>(Item))
	{
		FRotator TypeRotation = Weapon->GetWeaponTypeRotation();
		ItemTransform.SetRotation(FQuat(TypeRotation) * ItemTransform.GetRotation());
	}
	
	Item->SetActorRelativeTransform(ItemTransform);
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemComponent, EquippedItem);
}
