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

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("Target")));
	CurrentTarget = Target;

	if (Target == nullptr)
	{
		RunTargetPriorityQuery();
		return;
	}

	if (AT8AICharacter* TargetAI = Cast<AT8AICharacter>(Target))
	{
		if (TargetAI->IsDead())
		{
			BB->ClearValue(TEXT("Target"));
			RunTargetPriorityQuery();
			return;
		}
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
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		TargetQueryTemplate,
		this,
		EEnvQueryRunMode::AllMatching,
		nullptr
	);

	if (!QueryInstance)
	{
		return;
	}

	QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AT8AIController::OnTargetPriorityQueryFinished);
}



void AT8AIController::OnTargetPriorityQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!QueryInstance || QueryStatus != EEnvQueryStatus::Success)
	{
		return;
	}

	TArray<AActor*> FoundActors;
	QueryInstance->GetQueryResultsAsActors(FoundActors);

	UE_LOG(LogTemp, Warning, TEXT("[TargetPriority] Found %d candidates"), FoundActors.Num());

	AT8AICharacter* SelfChar = Cast<AT8AICharacter>(GetPawn());
	if (!SelfChar) return;

	AActor* BestTarget = nullptr;

	for (AActor* Actor : FoundActors)
	{
		if (SelfChar->IsEnemy(Actor))
		{
			BestTarget = Actor;
			break;
		}
	}

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	if (BestTarget)
	{
		BB->SetValueAsObject(TEXT("Target"), BestTarget);

		UObject* SetTarget = BB->GetValueAsObject(TEXT("Target"));
	}
	else
	{
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
