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
		UE_LOG(LogTemp, Warning, TEXT("[BTTask_FindNearbyWeapon] 중복 쿼리 방지로 실행 차단"));
		return EBTNodeResult::Failed;
	}

	UE_LOG(LogTemp, Warning, TEXT("[BTTask_FindNearbyWeapon] ExecuteTask 진입"));

	CachedOwnerComp = &OwnerComp;
	bQueryInProgress = true;
	this->AddToRoot();

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon || !WeaponSearchQuery)
	{
		bQueryInProgress = false;
		RemoveFromRoot();
		UE_LOG(LogTemp, Error, TEXT("[BTTask_FindNearbyWeapon] AICon 또는 Query 없음"));
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
	{
		bQueryInProgress = false;
		RemoveFromRoot();
		UE_LOG(LogTemp, Error, TEXT("[BTTask_FindNearbyWeapon] Pawn 없음"));
		return EBTNodeResult::Failed;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		Pawn, WeaponSearchQuery, Pawn, EEnvQueryRunMode::SingleResult, nullptr);

	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UT8BTTask_FindNearbyWeapon::OnQueryFinished);
		UE_LOG(LogTemp, Warning, TEXT("[BTTask_FindNearbyWeapon] EQS 쿼리 실행됨"));
		return EBTNodeResult::InProgress;
	}

	bQueryInProgress = false;
	RemoveFromRoot();
	UE_LOG(LogTemp, Error, TEXT("[BTTask_FindNearbyWeapon] 쿼리 인스턴스 실패"));
	return EBTNodeResult::Failed;
}

void UT8BTTask_FindNearbyWeapon::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (this == nullptr || !IsValid(this)) return;

	if (!CachedOwnerComp.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[BTTask_FindNearbyWeapon] CachedOwnerComp 유효하지 않음"));
		bQueryInProgress = false;
		RemoveFromRoot();
		return;
	}

	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BTTask_FindNearbyWeapon] 쿼리 실패"));
		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Failed);
		bQueryInProgress = false;
		RemoveFromRoot();
		return;
	}

	TArray<AActor*> Results;
	QueryInstance->GetQueryResultsAsActors(Results);

	UE_LOG(LogTemp, Warning, TEXT("[BTTask_FindNearbyWeapon] 결과 수: %d"), Results.Num());

	if (Results.Num() > 0)
	{
		AActor* Found = Results[0];
		UE_LOG(LogTemp, Warning, TEXT("[BTTask_FindNearbyWeapon] 발견한 무기: %s"), *GetNameSafe(Found));

		CachedOwnerComp->GetBlackboardComponent()->SetValueAsObject(TargetWeaponKey.SelectedKeyName, Found);

		UE_LOG(LogTemp, Warning, TEXT("[BTTask_FindNearbyWeapon] Blackboard '%s'에 '%s' 저장 완료"),
			*TargetWeaponKey.SelectedKeyName.ToString(),
			*GetNameSafe(Found));

		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BTTask_FindNearbyWeapon] 결과 없음"));
		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Failed);
	}

	bQueryInProgress = false;
	RemoveFromRoot();
}
