#include "AI/T8BTTask_FindNearbyWeapon.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UT8BTTask_FindNearbyWeapon::UT8BTTask_FindNearbyWeapon()
{
	NodeName = "Find Nearby Weapon";
	bNotifyTick = false;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UT8BTTask_FindNearbyWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (bQueryInProgress)
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerComp = &OwnerComp;
	bQueryInProgress = true;
	this->AddToRoot();

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon || !WeaponSearchQuery)
	{
		bQueryInProgress = false;
		RemoveFromRoot();
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
	{
		bQueryInProgress = false;
		RemoveFromRoot();
		return EBTNodeResult::Failed;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		Pawn, WeaponSearchQuery, Pawn, EEnvQueryRunMode::SingleResult, nullptr);

	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UT8BTTask_FindNearbyWeapon::OnQueryFinished);
		return EBTNodeResult::InProgress;
	}

	bQueryInProgress = false;
	RemoveFromRoot();
	return EBTNodeResult::Failed;
}

void UT8BTTask_FindNearbyWeapon::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (this == nullptr || !IsValid(this)) return;

	if (!CachedOwnerComp.IsValid())
	{
		bQueryInProgress = false;
		RemoveFromRoot();
		return;
	}

	if (QueryStatus != EEnvQueryStatus::Success)
	{
		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Failed);
		bQueryInProgress = false;
		RemoveFromRoot();
		return;
	}

	TArray<AActor*> Results;
	QueryInstance->GetQueryResultsAsActors(Results);

	if (Results.Num() > 0)
	{
		AActor* Found = Results[0];
		UE_LOG(LogTemp, Warning, TEXT("[BTTask_FindNearbyWeapon] 발견한 무기: %s"), *GetNameSafe(Found));

		CachedOwnerComp->GetBlackboardComponent()->SetValueAsObject(TargetWeaponKey.SelectedKeyName, Found);

		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
	}
	else
	{
		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Failed);
	}

	bQueryInProgress = false;
	RemoveFromRoot();
}
