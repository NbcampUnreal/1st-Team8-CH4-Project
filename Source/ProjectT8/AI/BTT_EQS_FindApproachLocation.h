#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BTT_EQS_FindApproachLocation.generated.h"

UCLASS()
class PROJECTT8_API UBTT_EQS_FindApproachLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_EQS_FindApproachLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "EQS")
	class UEnvQuery* EQSQuery;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector LocationKey;

private:
	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UPROPERTY()
	AActor* CachedQuerier;

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;
};
