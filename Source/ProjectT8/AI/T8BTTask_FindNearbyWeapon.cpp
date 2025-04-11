#include "AI/T8BTTask_FindNearbyWeapon.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UT8BTTask_FindNearbyWeapon::UT8BTTask_FindNearbyWeapon()
{
	NodeName = "Find Nearby Weapon";
}

EBTNodeResult::Type UT8BTTask_FindNearbyWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon || !WeaponSearchQuery) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();

	auto* QueryInstance = UEnvQueryManager::RunEQSQuery(
		Pawn, WeaponSearchQuery, Pawn, EEnvQueryRunMode::SingleResult, nullptr
	);

	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UT8BTTask_FindNearbyWeapon::OnQueryFinished);
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UT8BTTask_FindNearbyWeapon::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!CachedOwnerComp || QueryStatus != EEnvQueryStatus::Success)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
		return;
	}

	TArray<AActor*> Results;
	QueryInstance->GetQueryResultsAsActors(Results);
	
	if (Results.Num() > 0)
	{
		CachedOwnerComp->GetBlackboardComponent()->SetValueAsObject(TargetWeaponKey.SelectedKeyName, Results[0]);
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
	}
}
