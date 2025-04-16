#include "BTT_EQS_FindApproachLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"

UBTT_EQS_FindApproachLocation::UBTT_EQS_FindApproachLocation()
{
	NodeName = "Run EQS: Find Approach Location";
}

EBTNodeResult::Type UBTT_EQS_FindApproachLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !EQSQuery) return EBTNodeResult::Failed;

	CachedQuerier = AIController;
	CachedOwnerComp = &OwnerComp;

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		AIController,
		EQSQuery,
		AIController,
		EEnvQueryRunMode::SingleResult,
		nullptr
	);

	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UBTT_EQS_FindApproachLocation::OnQueryFinished);
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTT_EQS_FindApproachLocation::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!QueryInstance || QueryStatus != EEnvQueryStatus::Success) return;

	TArray<FVector> Locations;
	QueryInstance->GetQueryResultsAsLocations(Locations);
	if (Locations.Num() == 0 || !CachedQuerier || !CachedOwnerComp) return;

	AAIController* AIController = Cast<AAIController>(CachedQuerier);
	if (!AIController) return;

	UBlackboardComponent* BB = AIController->GetBlackboardComponent();
	if (!BB) return;

	BB->SetValueAsVector(LocationKey.SelectedKeyName, Locations[0]);
	UE_LOG(LogTemp, Log, TEXT("[EQS] 이동 위치: %s"), *Locations[0].ToString());

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
