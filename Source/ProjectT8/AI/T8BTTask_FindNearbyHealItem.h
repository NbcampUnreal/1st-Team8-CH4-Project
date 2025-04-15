#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "T8BTTask_FindNearbyHealItem.generated.h"

UCLASS()
class PROJECTT8_API UT8BTTask_FindNearbyHealItem : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UT8BTTask_FindNearbyHealItem();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UPROPERTY(EditAnywhere, Category = "EQS")
	UEnvQuery* HealItemSearchQuery;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetHealItemKey;

private:
	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;
};
