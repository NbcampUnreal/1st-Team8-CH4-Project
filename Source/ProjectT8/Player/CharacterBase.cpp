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
#include "GameFramework/GameMode/T8GameMode.h"
#include "GameFramework/Common/CustomGameState.h"
#include "GameFramework/Common/T8GameInstance.h"
#include "Player/Customize/FCharacterAppearanceData.h"


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

	

	if (AttributeSet)
	{
		AttributeSet->OnHealthChanged.AddDynamic(this, &ACharacterBase::HandleHealthChanged);
	}

	if (ACustomGameState* GameState = GetWorld()->GetGameState<ACustomGameState>())
	{
		HandleAppearanceByPhase(GameState->CurPhase);

		// 게임 상태 변경 이벤트 구독
		GameState->OnPhaseChanged.AddDynamic(this, &ACharacterBase::OnGamePhaseChanged);
		
	}

	StatusEffectTags = {
		FGameplayTag::RequestGameplayTag("State.Poisoned"),
		FGameplayTag::RequestGameplayTag("State.Burning"),
		FGameplayTag::RequestGameplayTag("State.Shocked"),
		FGameplayTag::RequestGameplayTag("State.Blinded"),
	};

	RegisterStatusEffectDelegates();

	// 초기 설정
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
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		UE_LOG(LogTemp, Warning, TEXT("쇽 쇽 쇽!!! 이동 불가"));
	}

	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Blinded")))
	{
		UE_LOG(LogTemp, Warning, TEXT("플래시 플래시!!!"));
		if (!FlashWidgetInstance && FlashWidgetClass)
		{
			FlashWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), FlashWidgetClass);
			if (FlashWidgetInstance)
			{
				FlashWidgetInstance->AddToViewport();
				UE_LOG(LogTemp, Warning, TEXT("플래시 위젯 생성"));
			}
		}
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

	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Shocked")))
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("쇼크 해제 이동 가능"));
	}

	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Blinded")))
	{
		if (FlashWidgetInstance)
		{
			FlashWidgetInstance->RemoveFromParent();
			FlashWidgetInstance = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("플래시 위젯 제거"));
		}
	}


}



void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, AbilitySystemComponent);
	DOREPLIFETIME(ACharacterBase, CombatComponent);
	DOREPLIFETIME(ACharacterBase, ItemComponent);
	DOREPLIFETIME(ACharacterBase, bIsDead);
	DOREPLIFETIME(ACharacterBase, TeamNumber);
	DOREPLIFETIME(ACharacterBase, PlayerDisplayName);
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

	float FinalSpeed = RunSpeed;
	if (bIsSpeedBoosted)
	{
		FinalSpeed *= SpeedBoostRatio;
	}

	GetCharacterMovement()->MaxWalkSpeed = FinalSpeed;
}

void ACharacterBase::SprintEnd()
{
	bIsRunning = false;

	float FinalSpeed = WalkSpeed;
	if (bIsSpeedBoosted)
	{
		FinalSpeed *= SpeedBoostRatio;
	}

	GetCharacterMovement()->MaxWalkSpeed = FinalSpeed;
}

void ACharacterBase::SpeedUpStart(float SpeedRatio)
{
	bIsSpeedBoosted = true;
	SpeedBoostRatio = SpeedRatio;

	if (bIsRunning)
		SprintStart();
	else
		SprintEnd();
}

void ACharacterBase::SpeedUpEnd()
{
	bIsSpeedBoosted = false;

	if (bIsRunning)
		SprintStart();
	else
		SprintEnd();
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
	InitAbilityActorInfo();
}

EGamePhase ACharacterBase::GetCurrentGamePhase() const
{
	if (const ACustomGameState* GameState = GetWorld()->GetGameState<ACustomGameState>())
	{
		return GameState->CurPhase;
	}
	return EGamePhase::None;
}

void ACharacterBase::UpdatePlayerName()
{
	if (!StatusWidget) return;

	if (AT8PlayerState* PS = GetPlayerState<AT8PlayerState>())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState Found for Character: %s"), *GetName());
		UE_LOG(LogTemp, Warning, TEXT("PersonaName: %s"), *PS->PersonaName);
		StatusWidget->SetPlayerName(PS->PersonaName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerState is NULL for Character: %s"), *GetName());
		// PlayerState가 없는 경우 기본 이름 설정
		StatusWidget->SetPlayerName(TEXT("Unknown"));
	}
}

void ACharacterBase::HandleAppearanceByPhase(EGamePhase Phase)
{
	switch (Phase)
	{
	case EGamePhase::Room:
		if (UCharacterAppearanceSubsystem* Sub = GetGameInstance()->GetSubsystem<UCharacterAppearanceSubsystem>())
		{
			Sub->LoadAppearance();
			ApplyApperance(Sub->CachedAppearanceData);
		}
		break;
	case EGamePhase::Lobby:
	case EGamePhase::Playing:
	case EGamePhase::Result:
		if (APlayerState* PS = GetPlayerState())
		{
			FCharacterAppearanceData& Data = Cast<AT8PlayerState>(PS)->ApperanceData;
			ApplyApperance(Data);
		}
		break;
	default:
		break;
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
			UpdatePlayerName();  // 플레이어 이름 업데이트
			StatusWidget->SetHealthBarVisibility(GetCurrentGamePhase() == EGamePhase::Playing);
		}
	}
}

void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();

	// 플레이어 외형 갱신
	if (ACustomGameState* GameState = GetWorld()->GetGameState<ACustomGameState>())
	{
		HandleAppearanceByPhase(GameState->CurPhase);
	}
}

void ACharacterBase::OnGamePhaseChanged(EGamePhase NewPhase)
{
	if (FloatingStatusWidget && StatusWidget)
	{
		// HP바의 가시성만 게임 상태에 따라 조절
		StatusWidget->SetHealthBarVisibility(NewPhase == EGamePhase::Playing);
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

void ACharacterBase::Die()
{
	if (HasAuthority())
	{
		bIsDead = true;
		MulticastDie();

		// 게임모드에 죽음 알림
		if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
		{
			if (AT8GameMode* T8GameMode = Cast<AT8GameMode>(GameMode))
			{
				T8GameMode->NotifyPlayerDeath(this);
			}
		}
	}
}

void ACharacterBase::MulticastDie_Implementation()
{
	// 모든 클라이언트에서 실행될 죽음 처리
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->StopMovementImmediately();
	}

	// 메인 캐릭터 메시의 애니메이션 중지
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.0f);
		AnimInstance->bReceiveNotifiesFromLinkedInstances = false;
	}

	// 장착된 장비들의 애니메이션도 중지
	TArray<USkeletalMeshComponent*> EquipmentMeshes = {
		HeadMesh, AccessoryMesh, GlovesMesh, TopMesh, BottomMesh, ShoesMesh
	};

	for (USkeletalMeshComponent* EquipmentMesh : EquipmentMeshes)
	{
		if (EquipmentMesh)
		{
			if (UAnimInstance* EquipAnimInstance = EquipmentMesh->GetAnimInstance())
			{
				EquipAnimInstance->StopAllMontages(0.0f);
				EquipAnimInstance->bReceiveNotifiesFromLinkedInstances = false;
			}
			EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			EquipmentMesh->SetSimulatePhysics(true);
			EquipmentMesh->SetEnableGravity(true);
		}
	}

	// 콜리전 설정
	if (GetCapsuleComponent())
	{
		// 캡슐 콜리전은 블록으로 유지하되 크기를 줄임
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		
		// 캡슐 크기를 줄여서 바닥에 눕게 함
		GetCapsuleComponent()->SetCapsuleHalfHeight(20.0f);
	}

	// 메시에 물리 시뮬레이션 활성화
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetEnableGravity(true);
		GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("root"), true, true);
	}

	if (ItemComponent)
	{
		if (ABaseItem* EquippedItem = ItemComponent->GetEquippedItem())
		{
			ItemComponent->DropItemToWorld();
		}
	}

	OnCharacterDeath.Broadcast(this);
}

void ACharacterBase::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (StatusWidget)
	{
		StatusWidget->UpdateHealthBar(CurrentHealth, MaxHealth);
	}
}