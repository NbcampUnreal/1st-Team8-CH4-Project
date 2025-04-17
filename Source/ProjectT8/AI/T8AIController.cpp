#include "AI/T8AIController.h"
#include "AI/T8AICharacter.h"
#include "Player/CharacterBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"

AT8AIController::AT8AIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AT8AIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}

	RunTargetPriorityQuery();
}


void AT8AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	this->CurrentTarget = Cast<AActor>(BB->GetValueAsObject(TEXT("Target")));

	if (this->CurrentTarget == nullptr || !LineOfSightTo(this->CurrentTarget))
	{
		BB->ClearValue(TEXT("Target"));
		RequestFindNewTarget();
	}
}


AActor* AT8AIController::GetTarget() const
{
	if (Blackboard)
	{
		return Cast<AActor>(Blackboard->GetValueAsObject(TEXT("Target")));
	}
	return nullptr;
}

void AT8AIController::RunWeaponSearchQuery()
{
	if (!WeaponQueryTemplate) return;

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		WeaponQueryTemplate,
		this,
		EEnvQueryRunMode::SingleResult,
		nullptr
	);

	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AT8AIController::OnWeaponQueryFinished);
	}
}

void AT8AIController::OnWeaponQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!QueryInstance) return;

	TArray<AActor*> FoundItems;
	QueryInstance->GetQueryResultsAsActors(FoundItems);

	if (FoundItems.Num() > 0)
	{
		GetBlackboardComponent()->SetValueAsObject(TEXT("NearbyWeapon"), FoundItems[0]);
	}
	else
	{
		GetBlackboardComponent()->ClearValue(TEXT("NearbyWeapon"));
	}
}

void AT8AIController::RunTargetPriorityQuery()
{
	if (!PriorityTargetQueryTemplate) return;

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		PriorityTargetQueryTemplate,
		this,
		EEnvQueryRunMode::AllMatching,
		nullptr
	);

	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AT8AIController::OnTargetPriorityQueryFinished);
	}
}

void AT8AIController::OnTargetPriorityQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!QueryInstance || QueryStatus != EEnvQueryStatus::Success) return;

	TArray<AActor*> FoundActors;
	QueryInstance->GetQueryResultsAsActors(FoundActors);

	AT8AICharacter* SelfChar = Cast<AT8AICharacter>(GetPawn());
	if (!SelfChar) return;

	AActor* BestTarget = nullptr;
	float LowestHealth = MAX_flt;

	for (AActor* Actor : FoundActors)
	{
		if (!SelfChar->IsEnemy(Actor)) continue;

		ACharacterBase* TargetCharacter = Cast<ACharacterBase>(Actor);
		if (TargetCharacter && TargetCharacter->IsDead()) continue;

		if (SelfChar->LastDamager == Actor)
		{
			float TimeSinceDamaged = GetWorld()->GetTimeSeconds() - SelfChar->LastDamagerSetTime;
			if (TimeSinceDamaged <= SelfChar->LastDamagerMemoryTime)
			{
				BestTarget = Actor;
				break;
			}
		}

		if (!BestTarget)
		{
			BestTarget = Actor;
		}
	}

	float Now = GetWorld()->GetTimeSeconds();
	if (!CurrentTarget) LastTargetSetTime = 0.f;

	if (CurrentTarget && (Now - LastTargetSetTime) < TargetHoldTime)
	{
		GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), CurrentTarget);
	}
	else
	{
		CurrentTarget = BestTarget;
		LastTargetSetTime = Now;

		if (BestTarget)
		{
			GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), BestTarget);
		}
	}
}


void AT8AIController::RequestFindNewTarget()
{
	if (!TargetSearchQuery) return;

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	UEnvQueryInstanceBlueprintWrapper* QueryInstance =
		UEnvQueryManager::RunEQSQuery(
			this,
			TargetSearchQuery,
			ControlledPawn,
			EEnvQueryRunMode::SingleResult,
			nullptr);

	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AT8AIController::OnTargetPriorityQueryFinished);
	}
}
