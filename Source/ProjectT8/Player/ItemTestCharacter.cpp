#include "Player/ItemTestCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/OverlapResult.h" //* 추가 * +3줄
#include "CollisionQueryParams.h"
#include "Item/BaseItem.h"
#include "Net/UnrealNetwork.h"


AItemTestCharacter::AItemTestCharacter()
{
	EffectComponent = CreateDefaultSubobject<UEffectComponent>(TEXT("EffectComponent"));	// * 추가 * +2줄
}

void AItemTestCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Warning, TEXT("InputMappingContext 등록 완료 (ItemTestCharacter)"));
		}
	}
}

void AItemTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AItemTestCharacter::TryInteract);// * 추가 *
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started, this, &AItemTestCharacter::UseItem);// * 추가 *
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
	UE_LOG(LogTemp, Warning, TEXT(">> SetupPlayerInputComponent 실행됨"));
	if (InteractAction)
	{
		UE_LOG(LogTemp, Warning, TEXT(">> InteractAction 유효함: %s"), *InteractAction->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(">> InteractAction 애셋이 없음!"));
	}
}


void AItemTestCharacter::ModifyHealth(float Amount)	// * 추가 * 하위 코드 전부
{
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Character Health: %f"), Health);
}

void AItemTestCharacter::PickupItem(ABaseItem* Item)
{
	if (EquippedItem)
	{
		EquippedItem->Destroy();
	}
	EquippedItem = Item;
	if (EquippedItem)
	{
		UE_LOG(LogTemp, Log, TEXT("Picking up item: %s"), *GetNameSafe(EquippedItem));
		EquippedItem->SetOwner(this);

		EquippedItem->SetActorEnableCollision(false);
		EquippedItem->GetItemMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EquippedItem->GetItemMesh()->SetSimulatePhysics(false);
		if (HasAuthority())
		{
			EquippedItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));
		}

	}
}

void AItemTestCharacter::Server_PickupItem_Implementation(ABaseItem* Item)
{
	PickupItem(Item);
}

void AItemTestCharacter::TryInteract()
{
	UE_LOG(LogTemp, Log, TEXT("TryInteract triggered"));
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(250.0f), Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AActor* OverlappedActor = Overlap.GetActor())
			{
				if (OverlappedActor->Implements<UInteractable>())
				{
					if (HasAuthority())
					{
						IInteractable::Execute_Interact(OverlappedActor, this);
					}
					else
					{
						Server_Interact(OverlappedActor); // 새로운 RPC로 서버에 요청
					}
					break;
				}
			}
		}
	}
}

void AItemTestCharacter::UseItem()
{
	if (HasAuthority())
	{
		if (EquippedItem)
		{
			EquippedItem->Use(this);
		}
	}
	else 
	{
		Server_UseItem();
	}
}
void AItemTestCharacter::Server_UseItem_Implementation()
{
	UseItem();
}
void AItemTestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemTestCharacter, EquippedItem);
}

void AItemTestCharacter::Server_Interact_Implementation(AActor* InteractableActor)
{
	if (InteractableActor && InteractableActor->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(InteractableActor, this);
	}
}