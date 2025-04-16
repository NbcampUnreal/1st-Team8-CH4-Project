#include "AI/T8BTTask_FindNearbyHealItem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UT8BTTask_FindNearbyHealItem::UT8BTTask_FindNearbyHealItem()
{
	NodeName = "Find Nearby Heal Item";
	bNotifyTick = false;
}

EBTNodeResult::Type UT8BTTask_FindNearbyHealItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon || !HealItemSearchQuery)
	{
		UE_LOG(LogTemp, Error, TEXT("[FindNearbyHealItem] AIController 또는 쿼리 없음"));
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[FindNearbyHealItem] Pawn 없음"));
		return EBTNodeResult::Failed;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		Pawn,
		HealItemSearchQuery,
		Pawn,
		EEnvQueryRunMode::SingleResult,
		nullptr
	);

	if (QueryInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FindNearbyHealItem] EQS 쿼리 실행됨"));
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UT8BTTask_FindNearbyHealItem::OnQueryFinished);
		return EBTNodeResult::InProgress;
	}

	UE_LOG(LogTemp, Error, TEXT("[FindNearbyHealItem] 쿼리 인스턴스 생성 실패"));
	return EBTNodeResult::Failed;
}

void UT8BTTask_FindNearbyHealItem::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!CachedOwnerComp || QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FindNearbyHealItem] 쿼리 실패"));
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
		return;
	}

	TArray<AActor*> Results;
	QueryInstance->GetQueryResultsAsActors(Results);
	UE_LOG(LogTemp, Warning, TEXT("[FindNearbyHealItem] 결과 수: %d"), Results.Num());

	if (Results.Num() > 0)
	{
		AActor* FoundItem = Results[0];
		UE_LOG(LogTemp, Warning, TEXT("[FindNearbyHealItem] 발견된 힐 아이템: %s"), *FoundItem->GetName());

		CachedOwnerComp->GetBlackboardComponent()->SetValueAsObject(TargetHealItemKey.SelectedKeyName, FoundItem);
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[FindNearbyHealItem] 주변 힐 아이템 없음"));
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
	}
}
