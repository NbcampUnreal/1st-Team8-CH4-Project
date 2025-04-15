#include "CharacterBase.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Common/T8PlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "Item/BaseItem.h"
#include "Engine/OverlapResult.h"
#include "Blueprint/UserWidget.h"
#include "Component/CombatComponent.h"
#include "GAS/CharacterAttributeSet.h"
#include "Component/ItemComponent.h"
#include "Player/Customize/CharacterAppearanceSubsystem.h"
#include "Item/Weapon.h"
#include "UI/FloatingStatusWidget.h"
#include "Components/WidgetComponent.h"


// Constructor
ACharacterBase::ACharacterBase()
{
	// GAS
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));

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

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(GetMesh());
	HeadMesh->SetLeaderPoseComponent(GetMesh());

	AccessoryMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AccessoryMesh"));
	AccessoryMesh->SetupAttachment(GetMesh());
	AccessoryMesh->SetLeaderPoseComponent(GetMesh());

	GlovesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GlovesMesh"));
	GlovesMesh->SetupAttachment(GetMesh());
	GlovesMesh->SetLeaderPoseComponent(GetMesh());

	TopMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TopMesh"));
	TopMesh->SetupAttachment(GetMesh());
	TopMesh->SetLeaderPoseComponent(GetMesh());

	BottomMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BottomMesh"));
	BottomMesh->SetupAttachment(GetMesh());
	BottomMesh->SetLeaderPoseComponent(GetMesh());

	ShoesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShoesMesh"));
	ShoesMesh->SetupAttachment(GetMesh());
	ShoesMesh->SetLeaderPoseComponent(GetMesh());

	// 플로팅 위젯 컴포넌트 생성
	FloatingStatusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatingStatusWidget"));
	FloatingStatusWidget->SetupAttachment(GetRootComponent());
	FloatingStatusWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));  // 캐릭터 위에 위치
	FloatingStatusWidget->SetWidgetSpace(EWidgetSpace::Screen);  // 항상 화면을 향하도록
	FloatingStatusWidget->SetDrawSize(FVector2D(150.0f, 50.0f));  // 위젯 크기

	// 네트워크 복제 설정
	FloatingStatusWidget->SetIsReplicated(true);
	FloatingStatusWidget->SetVisibility(true);
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
	if (ItemComponent)
	{
		ItemComponent->Init(this);
	}
}

void ACharacterBase::ApplyApperance(const FCharacterAppearanceData& Data)
{
	UCharacterAppearanceSubsystem* ApperanceSybsystem = GetGameInstance()->GetSubsystem<UCharacterAppearanceSubsystem>();
	if (!ApperanceSybsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("ApperanceSubsystem is null!"));
		return;
	}

	// 각 부위별로 메시 적용 시도
	if (USkeletalMesh* HeadSkeletalMesh = ApperanceSybsystem->GetCostumeMeshByID(Data.HeadID))
	{
		if (HeadMesh)
		{
			HeadMesh->SetSkeletalMesh(HeadSkeletalMesh);
		}
	}
	if (USkeletalMesh* AccessorySkeletalMesh = ApperanceSybsystem->GetCostumeMeshByID(Data.AccessoryID))
	{
		if (AccessoryMesh)
		{
			AccessoryMesh->SetSkeletalMesh(AccessorySkeletalMesh);
		}
	}
	if (USkeletalMesh* GlovesSkeletalMesh = ApperanceSybsystem->GetCostumeMeshByID(Data.GlovesID))
	{
		if (GlovesMesh)
		{
			GlovesMesh->SetSkeletalMesh(GlovesSkeletalMesh);
		}
	}
	if (USkeletalMesh* TopSkeletalMesh = ApperanceSybsystem->GetCostumeMeshByID(Data.TopID))
	{
		if (TopMesh)
		{
			TopMesh->SetSkeletalMesh(TopSkeletalMesh);
		}
	}
	if (USkeletalMesh* BottomSkeletalMesh = ApperanceSybsystem->GetCostumeMeshByID(Data.BottomID))
	{
		if (BottomMesh)
		{
			BottomMesh->SetSkeletalMesh(BottomSkeletalMesh);
		}
	}
	if (USkeletalMesh* ShoesSkeletalMesh = ApperanceSybsystem->GetCostumeMeshByID(Data.ShoesID))
	{
		if (ShoesMesh)
		{
			ShoesMesh->SetSkeletalMesh(ShoesSkeletalMesh);
		}
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

	// PlayerState가 있을 때만 외형 적용 시도
	if (AT8PlayerState* PS = GetPlayerState<AT8PlayerState>())
	{
		ApplyApperance(PS->ApperanceData);
	}

	InitializeFloatingStatusWidget();
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
	DOREPLIFETIME(ACharacterBase, CombatComponent);
	DOREPLIFETIME(ACharacterBase, ItemComponent);
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

void ACharacterBase::UseItem()
{
	if (ItemComponent)
	{
		ItemComponent->UseEquippedItem();
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
		// 가장 가까운 거리에 있는 Actor를 확인해서 인터렉션 진행
		AActor* ClosestInteractable = nullptr;
		float ClosestDistSq = TNumericLimits<float>::Max();

		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AActor* OverlappedActor = Overlap.GetActor())
			{
				if (OverlappedActor->Implements<UInteractable>())
				{
					float DistSq = FVector::DistSquared(OverlappedActor->GetActorLocation(), GetActorLocation());
					if (DistSq < ClosestDistSq)
					{
						ClosestDistSq = DistSq;
						ClosestInteractable = OverlappedActor;
					}
				}
			}
		}
		if (ClosestInteractable)
		{
			if (!HasAuthority())
			{
				Server_Interact(ClosestInteractable);
			}
			else
			{
				IInteractable::Execute_Interact(ClosestInteractable, this);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TryInteract Is Fail!!!"));
	}
}

void ACharacterBase::Server_Interact_Implementation(AActor* InteractableActor)
{
	IInteractable::Execute_Interact(InteractableActor, this);
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// PlayerState가 있다면 외형 적용
	if (AT8PlayerState* PS = GetPlayerState<AT8PlayerState>())
	{
		ApplyApperance(PS->ApperanceData);
	}
}

void ACharacterBase::InitializeFloatingStatusWidget()
{
	if (FloatingStatusWidget)
	{
		StatusWidget = Cast<UFloatingStatusWidget>(FloatingStatusWidget->GetUserWidgetObject());
		if (StatusWidget)
		{
			StatusWidget->SetOwnerCharacter(this);
			
			// 플레이어 이름 설정
			if (APlayerState* PS = GetPlayerState())
			{
				StatusWidget->SetPlayerName(PS->GetPlayerName());
			}
		}
	}
}

float ACharacterBase::GetHealth() const
{
	if (AbilitySystemComponent)
	{
		if (const UCharacterAttributeSet* CharacterAS = Cast<UCharacterAttributeSet>(AbilitySystemComponent->GetAttributeSet(UCharacterAttributeSet::StaticClass())))
		{
			return CharacterAS->GetHealth();
		}
	}
	return 0.0f;
}

float ACharacterBase::GetMaxHealth() const
{
	if (AbilitySystemComponent)
	{
		if (const UCharacterAttributeSet* CharacterAS = Cast<UCharacterAttributeSet>(AbilitySystemComponent->GetAttributeSet(UCharacterAttributeSet::StaticClass())))
		{
			return CharacterAS->GetMaxHealth();
		}
	}
	return 0.0f;
}

void ACharacterBase::UpdateHealthUI()
{
	if (StatusWidget && AbilitySystemComponent)
	{
		float CurrentHealth = GetHealth();
		float MaxHealth = GetMaxHealth();
		StatusWidget->UpdateHealthBar(CurrentHealth, MaxHealth);
	}
}