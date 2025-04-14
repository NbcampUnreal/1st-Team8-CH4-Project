#include "AI/T8AIController.h"
#include "AI/T8AICharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"

AT8AIController::AT8AIController()
{
}

void AT8AIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}

	RunTargetSearchQuery();
}


void AT8AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Blackboard && !Blackboard->GetValueAsObject(TEXT("TargetPlayer")))
	{
		RunTargetSearchQuery();
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



void AT8AIController::RunTargetSearchQuery()
{
	if (!TargetQueryTemplate) return;

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		TargetQueryTemplate,
		this,
		EEnvQueryRunMode::SingleResult,
		nullptr
	);

	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AT8AIController::OnEQSQueryFinished);
	}
}

void AT8AIController::OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!QueryInstance || !Blackboard) return;

	TArray<AActor*> FoundActors;
	QueryInstance->GetQueryResultsAsActors(FoundActors);

	if (FoundActors.Num() > 0)
	{
		AActor* TargetActor = FoundActors[0];
		Blackboard->SetValueAsObject(TEXT("TargetPlayer"), TargetActor);
	}
	else
	{
		Blackboard->ClearValue(TEXT("TargetPlayer"));
	}
}
