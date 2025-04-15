#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "T8BTTask_FindNearbyWeapon.generated.h"

UCLASS()
class PROJECTT8_API UT8BTTask_FindNearbyWeapon : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UT8BTTask_FindNearbyWeapon();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UPROPERTY(EditAnywhere, Category = "EQS")
	UEnvQuery* WeaponSearchQuery;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetWeaponKey;

private:
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	bool bQueryInProgress = false;
};
