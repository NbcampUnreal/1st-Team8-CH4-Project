#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "T8BTTask_UseHealItem.generated.h"

UCLASS()
class PROJECTT8_API UT8BTTask_UseHealItem : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UT8BTTask_UseHealItem();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector HealItemKey;
};
