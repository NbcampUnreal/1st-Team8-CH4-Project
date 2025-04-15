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

	this->CurrentTarget = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetPlayer")));

	if (this->CurrentTarget == nullptr || !LineOfSightTo(this->CurrentTarget))
	{
		UE_LOG(LogTemp, Warning, TEXT("타겟 상실: TargetPlayer 초기화"));
		BB->ClearValue(TEXT("TargetFromBB"));
		RequestFindNewTarget();
	}
}


AActor* AT8AIController::GetTargetPlayer() const
{
	if (Blackboard)
	{
		return Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetPlayer")));
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

	AActor* BestTarget = nullptr;
	float LowestHealth = MAX_flt;

	AT8AICharacter* SelfChar = Cast<AT8AICharacter>(GetPawn());

	for (AActor* Actor : FoundActors)
	{
		ACharacterBase* Target = Cast<ACharacterBase>(Actor);
		if (!Target /*|| Target->IsDead()*/) continue;

		//복수 타겟 우선
		if (SelfChar && SelfChar->LastDamager == Target)
		{
			float TimeSinceDamaged = GetWorld()->GetTimeSeconds() - SelfChar->LastDamagerSetTime;

			if (TimeSinceDamaged <= SelfChar->LastDamagerMemoryTime)
			{
				BestTarget = Target;
				break;
			}
		}

		//체력 비교 (TODO: GetHealth 구현 후 주석 해제)
		/*
		float Health = Target->GetHealth();
		if (Health < LowestHealth)
		{
			LowestHealth = Health;
			BestTarget = Target;
		}
		*/

		//임시 우선순위: 첫 유효 타겟
		if (!BestTarget)
		{
			BestTarget = Target;
		}
	}


	float Now = GetWorld()->GetTimeSeconds();

	if (!CurrentTarget /*|| CurrentTarget->IsDead()*/)
	{
		LastTargetSetTime = 0.f;
	}

	// 현재 타겟 유지 시간 안 지났으면 기존 타겟 유지
	if (CurrentTarget && (Now - LastTargetSetTime) < TargetHoldTime)
	{
		GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPlayer"), CurrentTarget);
	}
	else
	{
		// 새로운 타겟으로 교체
		CurrentTarget = BestTarget;
		LastTargetSetTime = Now;

		if (BestTarget)
		{
			GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPlayer"), BestTarget);
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
