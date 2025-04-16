#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "T8BTTask_UseWeapon.generated.h"

UCLASS()
class PROJECTT8_API UT8BTTask_UseWeapon : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UT8BTTask_UseWeapon();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
