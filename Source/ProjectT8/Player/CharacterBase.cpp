#include "CharacterBase.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "Item/BaseItem.h"
#include "Engine/OverlapResult.h"
#include "Blueprint/UserWidget.h"
#include "CombatComponent.h"
#include "GAS/CharacterAttributeSet.h"


// Constructor
ACharacterBase::ACharacterBase()
{
	// GAS
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	SetReplicates(true);
	SetReplicateMovement(true);
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bIsRunning = false;
}

// GAS
UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACharacterBase::InitAbilityActorInfo()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	if (CombatComponent)
	{
		CombatComponent->Init(this);
	}
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();

	StatusEffectTags = {
		FGameplayTag::RequestGameplayTag("State.Poisoned"),
		FGameplayTag::RequestGameplayTag("State.Burning"),
		FGameplayTag::RequestGameplayTag("State.Shocked"),
	};

	RegisterStatusEffectDelegates();
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterBase::Move);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACharacterBase::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACharacterBase::SprintEnd);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACharacterBase::Attack);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACharacterBase::TryInteract);
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started, this, &ACharacterBase::UseItem);
	}
}

void ACharacterBase::RegisterStatusEffectDelegates()
{
	for (const FGameplayTag& Tag : StatusEffectTags)
	{
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &ACharacterBase::OnStatusEffectTagChanged);
		}
	}
}

void ACharacterBase::OnStatusEffectTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[태그 적용] %s"), *Tag.ToString());
		ShowStatusWidget(Tag);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[태그 제거] %s"), *Tag.ToString());
		HideStatusWidget(Tag);
	}
}

void ACharacterBase::ShowStatusWidget(const FGameplayTag& Tag)
{
	if (!IsLocallyControlled()) return;

	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Poisoned")))
	{
		UE_LOG(LogTemp, Warning, TEXT("독 독 독!!!"));
	}

	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Burning")))
	{
		UE_LOG(LogTemp, Warning, TEXT("불 불 불!!!"));
		if (!BurnWidgetInstance && BurnWidgetClass)
		{
			BurnWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), BurnWidgetClass);
			if (BurnWidgetInstance)
			{
				BurnWidgetInstance->AddToViewport();
				UE_LOG(LogTemp, Warning, TEXT("불 위젯 생성"));
			}
		}
	}

	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Shocked")))
	{
		UE_LOG(LogTemp, Warning, TEXT("쇽 쇽 쇽!!!"));
	}
}

void ACharacterBase::HideStatusWidget(const FGameplayTag& Tag)
{
	if (!IsLocallyControlled()) return;

	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Burning")))
	{
		if (BurnWidgetInstance)
		{
			BurnWidgetInstance->RemoveFromParent();
			BurnWidgetInstance = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("불 위젯 제거"));
		}
	}
}



void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, AbilitySystemComponent);
	DOREPLIFETIME(ACharacterBase, EquippedItem);
	DOREPLIFETIME(ACharacterBase, CombatComponent);
}

// BeginPlay


// Controller & Input
void ACharacterBase::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}



void ACharacterBase::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACharacterBase::SprintStart()
{
	bIsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ACharacterBase::SprintEnd()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Attack
void ACharacterBase::Attack()
{
	if (CombatComponent)
	{
		CombatComponent->Attack();
	}
}

bool ACharacterBase::CanAttack() const
{
	return !AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Stunned"));
}

void ACharacterBase::PickupItem(ABaseItem* Item)
{
	if (EquippedItem)
	{
		EquippedItem->Destroy();
	}

	EquippedItem = Item;

	if (EquippedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Picking up item: %s"), *GetNameSafe(EquippedItem));
		EquippedItem->SetOwner(this);
		EquippedItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		EquippedItem->SetActorHiddenInGame(false);
		EquippedItem->SetActorEnableCollision(false);

		// 작업 필요
		CurrentDamageEffect = EquippedItem->GetAssociatedGameplayEffect();
	}
}

void ACharacterBase::UseItem()
{
	if (EquippedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UseItem Triggered"));
		EquippedItem->Use(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EquippedItem Is Null!!!"));
	}
}

void ACharacterBase::TryInteract()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->OverlapMultiByChannel(
		Overlaps, 
		GetActorLocation(), 
		FQuat::Identity, 
		ECC_Visibility, 
		FCollisionShape::MakeSphere(250.f), Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AActor* OverlappedActor = Overlap.GetActor())
			{
				if (OverlappedActor->Implements<UInteractable>())
				{
					IInteractable::Execute_Interact(OverlappedActor, this);
				}
				else
				{	
					Server_Interact(OverlappedActor);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TryInteract Is Fail!!!"));
	}
}

void ACharacterBase::Server_PickupItem_Implementation(ABaseItem* Item)
{
	PickupItem(Item);
}

void ACharacterBase::Server_Interact_Implementation(AActor* InteractableActor)
{
	if (InteractableActor && InteractableActor->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(InteractableActor, this);
	}
}
