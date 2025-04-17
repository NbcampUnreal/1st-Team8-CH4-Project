#include "AI/T8AICharacter.h"
#include "AI/T8AIController.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/Component/CombatComponent.h"
#include "Player/Component/ItemComponent.h"
#include "Item/BaseItem.h"
#include "UI/FloatingStatusWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/GameMode/T8GameMode.h"
#include "GameFramework/GameState/LobbyGameState.h"


AT8AICharacter::AT8AICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	bReplicates = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));

	TeamIndicator = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TeamIndicator"));
	TeamIndicator->SetupAttachment(RootComponent);
	TeamIndicator->SetHorizontalAlignment(EHTA_Center);
	TeamIndicator->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	TeamIndicator->SetText(FText::FromString("AI"));
	TeamIndicator->SetTextRenderColor(FColor::Cyan);
	TeamIndicator->SetWorldSize(40.0f);


	FloatingStatusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatingStatusWidget"));
	FloatingStatusWidget->SetupAttachment(GetRootComponent());
	FloatingStatusWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	FloatingStatusWidget->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingStatusWidget->SetDrawSize(FVector2D(150.0f, 50.0f));

	FloatingStatusWidget->SetIsReplicated(true);
	FloatingStatusWidget->SetVisibility(true);
}

void AT8AICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();

	if (AttributeSet)
	{
		AttributeSet->OnHealthChanged.AddDynamic(this, &AT8AICharacter::HandleHealthChanged);
	}

	InitializeFloatingStatusWidget();
	CachedAIController = Cast<AAIController>(GetController());

	if (InitEffectClass && AbilitySystemComponent)
	{
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);
		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(InitEffectClass, 1.0f, Context);
		if (Spec.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			UE_LOG(LogTemp, Warning, TEXT("초기화 이펙트 적용 완료"));
		}
	}

	if (FloatingStatusWidget)
	{
		StatusWidget = Cast<UFloatingStatusWidget>(FloatingStatusWidget->GetUserWidgetObject());
		if (StatusWidget)
		{
			StatusWidget->SetOwnerCharacter(Cast<ACharacterBase>(this));
		}
	}

	if (HasAuthority())
	{
		if (!DisplayName.IsEmpty()) 
		{
			if (ALobbyGameState* LobbyState = GetWorld()->GetGameState<ALobbyGameState>())
			{
				int32 AssignedTeam = LobbyState->GetTeamNumberForAI(DisplayName);
				SetTeamID(AssignedTeam);
			}
		}
		else
		{
			SetTeamID(TeamID);
		}
	}
}


void AT8AICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TeamIndicator)
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (CameraManager)
		{
			FVector CameraLocation = CameraManager->GetCameraLocation();
			FVector IndicatorLocation = TeamIndicator->GetComponentLocation();

			FVector ToCamera = (CameraLocation - IndicatorLocation);
			ToCamera.Z = 0.f;
			ToCamera.Normalize();

			FRotator LookRotation = ToCamera.Rotation();
			TeamIndicator->SetWorldRotation(LookRotation);
		}
	}


	if (!CachedAIController) return;

	bool bHasWeapon = (ItemComponent && ItemComponent->GetEquippedItem() != nullptr);

	if (UBlackboardComponent* BB = CachedAIController->GetBlackboardComponent())
	{
		BB->SetValueAsBool(TEXT("HasWeapon"), bHasWeapon);

		if (AttributeSet->GetHealth() < AttributeSet->GetMaxHealth() * 0.5f)
		{
			BB->SetValueAsBool("NeedHeal", true);
		}
		else
		{
			BB->SetValueAsBool("NeedHeal", false);
		}
	}
}

void AT8AICharacter::Attack()
{
	if (CombatComponent)
	{
		CombatComponent->Attack();
	}
}

void AT8AICharacter::UseItem()
{
	if (ItemComponent)
	{
		ItemComponent->UseEquippedItem();
	}
}

void AT8AICharacter::PerformAttackHitCheck()
{
	if (CombatComponent)
	{
		CombatComponent->HandleAttackNotify();
	}
}

void AT8AICharacter::ResetCanAttack()
{
	bCanAttack = true;
	bIsAttacking = false;

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		UBlackboardComponent* BB = AICon->GetBlackboardComponent();
		if (BB)
		{
			BB->SetValueAsBool(TEXT("IsAttacking"), false);
		}
	}
}

void AT8AICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AT8AICharacter, AbilitySystemComponent);
	DOREPLIFETIME(AT8AICharacter, CombatComponent);
	DOREPLIFETIME(AT8AICharacter, ItemComponent);
}

bool AT8AICharacter::IsEnemy(AActor* OtherActor) const
{
	if (!OtherActor || OtherActor == this) return false;

	int32 OtherTeam = -1;

	if (const AT8AICharacter* OtherAI = Cast<AT8AICharacter>(OtherActor))
	{
		OtherTeam = OtherAI->GetTeamID();
	}
	else if (const ACharacterBase* Player = Cast<ACharacterBase>(OtherActor))
	{
		OtherTeam = Player->TeamNumber;
	}

	return OtherTeam != TeamID;
}




int32 AT8AICharacter::GetTeamID() const
{
	return TeamID;
}

void AT8AICharacter::SetTeamID(int32 NewID)
{
	TeamID = NewID;

	if (TeamIndicator)
	{
		TeamIndicator->SetText(FText::FromString(FString::Printf(TEXT("T%d"), TeamID)));
		TeamIndicator->SetTextRenderColor(GetColorForTeam(TeamID));
	}
}

FColor AT8AICharacter::GetColorForTeam(int32 InTeamID) const
{
	switch (InTeamID)
	{
	case 0: return FColor::Blue;
	case 1: return FColor::Red;
	case 2: return FColor::Green;
	case 3: return FColor::Yellow;
	default: return FColor::White;
	}
}

void AT8AICharacter::SetDisplayName(const FString& NewName)
{
	DisplayName = NewName;

	if (TeamIndicator)
	{
		TeamIndicator->SetText(FText::FromString(NewName));
	}
}


UAbilitySystemComponent* AT8AICharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AT8AICharacter::InitAbilityActorInfo()
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


void AT8AICharacter::ApplyGameplayDamage(AActor* TargetActor)
{
	if (!TargetActor || !DamageEffectClass || !AbilitySystemComponent) return;

	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (!TargetCharacter) return;

	IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(TargetCharacter);
	if (!AbilityInterface) return;

	UAbilitySystemComponent* TargetASC = AbilityInterface->GetAbilitySystemComponent();
	if (!TargetASC) return;

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);
	if (Spec.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		UE_LOG(LogTemp, Warning, TEXT("AI가 %s에게 데미지 이펙트 적용"), *TargetActor->GetName());
	}
}

void AT8AICharacter::InitializeFloatingStatusWidget()
{
	if (FloatingStatusWidget)
	{
		StatusWidget = Cast<UFloatingStatusWidget>(FloatingStatusWidget->GetUserWidgetObject());
		if (StatusWidget)
		{
			StatusWidget->SetOwnerCharacter(this);
		}
	}
}

float AT8AICharacter::GetHealth() const
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

float AT8AICharacter::GetMaxHealth() const
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

void AT8AICharacter::UpdateHealthUI()
{
	if (StatusWidget && AbilitySystemComponent)
	{
		float CurrentHealth = GetHealth();
		float MaxHealth = GetMaxHealth();
		StatusWidget->UpdateHealthBar(CurrentHealth, MaxHealth);
	}
}

void AT8AICharacter::HandleHealthChanged(float NewHealth, float MaxHealth)
{
	if (!bIsDead && NewHealth <= 0.0f)
	{
		Die();
	}
}

void AT8AICharacter::Die()
{
	if (HasAuthority())
	{
		bIsDead = true;
		MulticastDie();

		// 게임모드에 AI 죽음 알림
		if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
		{
			if (AT8GameMode* T8GameMode = Cast<AT8GameMode>(GameMode))
			{
				T8GameMode->NotifyPlayerDeath(this);
			}
		}
	}
}

void AT8AICharacter::MulticastDie_Implementation()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->StopMovementImmediately();
	}

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.0f);
		AnimInstance->bReceiveNotifiesFromLinkedInstances = false;
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		GetCapsuleComponent()->SetCapsuleHalfHeight(20.0f);
	}

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
			TArray<UPrimitiveComponent*> WeaponComponents;
			EquippedItem->GetComponents<UPrimitiveComponent>(WeaponComponents);

			for (UPrimitiveComponent* Component : WeaponComponents)
			{
				if (Component)
				{
					Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					Component->SetSimulatePhysics(true);
					Component->SetEnableGravity(true);
				}
			}
		}
	}

	if (CachedAIController)
	{
		CachedAIController->GetBrainComponent()->StopLogic(TEXT("AI died"));
	}

	OnCharacterDeath.Broadcast(this);
}
