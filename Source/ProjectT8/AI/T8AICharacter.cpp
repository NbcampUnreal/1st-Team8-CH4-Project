#include "AI/T8AICharacter.h"
#include "AI/T8AIController.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/Component/CombatComponent.h"
#include "Player/Component/ItemComponent.h"
#include "Item/BaseItem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Camera/PlayerCameraManager.h"


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
	TeamIndicator->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	TeamIndicator->SetText(FText::FromString("T0"));
	TeamIndicator->SetTextRenderColor(FColor::White);
	TeamIndicator->SetWorldSize(30.0f);
}

void AT8AICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();

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
			FVector ToCamera = CameraLocation - TeamIndicator->GetComponentLocation();
			FRotator LookRotation = FRotationMatrix::MakeFromX(ToCamera).Rotator();

			LookRotation.Pitch = 0.f;
			LookRotation.Roll = 0.f;

			TeamIndicator->SetWorldRotation(LookRotation);
		}
	}

	if (!CachedAIController) return;

	bool bHasWeapon = (ItemComponent && ItemComponent->GetEquippedItem() != nullptr);

	if (UBlackboardComponent* BB = CachedAIController->GetBlackboardComponent())
	{
		BB->SetValueAsBool(TEXT("HasWeapon"), bHasWeapon);
	}

	UBlackboardComponent* BB = CachedAIController->GetBlackboardComponent();
	if (!bHasWeapon && BB && !BB->GetValueAsObject(TEXT("NearbyWeapon")))
	{
		if (AT8AIController* AICon = Cast<AT8AIController>(CachedAIController))
		{
			AICon->RunWeaponSearchQuery();
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


void AT8AICharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("AI 사망 처리 시작"));

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	DetachFromControllerPendingDestroy();

	SetLifeSpan(5.0f);
}

bool AT8AICharacter::IsEnemy(AActor* OtherActor) const
{
	if (!OtherActor || OtherActor == this) return false;

	if (!OtherActor->IsA<APawn>()) return false;

	if(const AT8AICharacter* OtherAI = Cast< AT8AICharacter>(OtherActor))
	{
		return OtherAI->GetTeamID() != GetTeamID();
	}

	return true;
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

		switch (TeamID)
		{
		case 0: TeamIndicator->SetTextRenderColor(FColor::Blue); break;
		case 1: TeamIndicator->SetTextRenderColor(FColor::Red); break;
		case 2: TeamIndicator->SetTextRenderColor(FColor::Green); break;
		default : TeamIndicator->SetTextRenderColor(FColor::White); break;
		}
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
		// TODO: 플레이어 파트와 협의되면 Init 활성화
		//CombatComponent->Init(this);
	}
	if (ItemComponent)
	{
		// TODO: 플레이어 파트와 협의되면 Init 활성화
		//ItemComponent->Init(this);
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

float AT8AICharacter::GetHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetHealth();
	}
	return 0.0f;
}
