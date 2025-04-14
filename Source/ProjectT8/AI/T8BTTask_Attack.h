#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "T8BTTask_Attack.generated.h"


UCLASS()
class PROJECTT8_API UT8BTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UT8BTTask_Attack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetKey;
};
